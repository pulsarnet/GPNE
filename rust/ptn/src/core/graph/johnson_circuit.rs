/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use super::strongly_connected::StronglyConnected;
use crate::net::vertex::VertexIndex;
use crate::net::PetriNet;
use std::collections::HashMap;

pub fn johnson_circuit(net: &PetriNet) -> Vec<Vec<VertexIndex>> {
    let nodes = net.places().len() + net.transitions().len();

    let mut vertex_to_index = HashMap::new();
    let mut index_to_vertex = HashMap::new();
    let mut index = 0usize;
    for place in net.places().keys().copied().map(VertexIndex::place) {
        vertex_to_index.insert(place, index);
        index_to_vertex.insert(index, place);
        index += 1;
    }

    for transition in net
        .transitions()
        .keys()
        .copied()
        .map(VertexIndex::transition)
    {
        vertex_to_index.insert(transition, index);
        index_to_vertex.insert(index, transition);
        index += 1;
    }

    let mut a_list = vec![vec![]; nodes];
    for edge in net.edges().directed() {
        let begin = vertex_to_index.get(&edge.begin()).expect("vertex");
        let end = vertex_to_index.get(&edge.end()).expect("vertex");
        a_list[*begin].push(*end);
    }

    for edge in net.edges().inhibitor().iter() {
        let begin = vertex_to_index.get(&edge.place()).expect("vertex");
        let end = vertex_to_index.get(&edge.transition()).expect("vertex");
        a_list[*begin].push(*end);
    }

    let mut result = vec![];
    let mut b_list = vec![vec![]; nodes];
    let mut blocked = vec![false; nodes];
    let mut stack = vec![];

    for s in 0..nodes {
        // strong components
        let components = StronglyConnected::components(&a_list);
        for component in components {
            if component.contains(&s) {
                for &v in &component {
                    blocked[v] = false;
                    b_list[v].clear();
                }

                circuit(
                    s,
                    &mut stack,
                    &a_list,
                    &mut b_list,
                    &mut blocked,
                    s,
                    &mut result,
                );
            }
        }

        a_list[s].clear();
    }

    result
        .into_iter()
        .map(|circuit| {
            circuit
                .into_iter()
                .map(|v| index_to_vertex.get(&v).expect("vertex").clone())
                .collect::<Vec<_>>()
        })
        .collect::<Vec<_>>()
}

fn circuit(
    v: usize,
    stack: &mut Vec<usize>,
    a_list: &[Vec<usize>],
    b_list: &mut [Vec<usize>],
    blocked: &mut [bool],
    s: usize,
    circuits: &mut Vec<Vec<usize>>,
) -> bool {
    let mut f = false;
    stack.push(v);
    blocked[v] = true;

    if let Some(neighbors) = a_list.get(v) {
        for &w in neighbors {
            if w == s {
                circuits.push(stack.clone());
                f = true;
            } else if !blocked[w] {
                if circuit(w, stack, a_list, b_list, blocked, s, circuits) {
                    f = true;
                }
            }
        }
    }

    if f {
        unblock(v, blocked, b_list);
    } else if let Some(neighbors) = a_list.get(v) {
        for &neighbor in neighbors {
            if !b_list[neighbor].contains(&v) {
                b_list[neighbor].push(v);
            }
        }
    }
    stack.pop();

    f
}

fn unblock(u: usize, blocked: &mut [bool], b_list: &mut [Vec<usize>]) {
    blocked[u] = false;
    while let Some(w) = b_list[u].pop() {
        if blocked[w] {
            unblock(w, blocked, b_list);
        }
    }
}

#[cfg(test)]
mod tests {
    use std::collections::{HashMap, HashSet};
    use indexmap::IndexMap;
    use crate::core::graph::johnson_circuit::johnson_circuit;
    use crate::net::place::Place;
    use crate::net::transition::Transition;
    use crate::net::vertex::{VertexIndex, VertexType};
    use crate::net::{DirectedEdge, PetriNet};

    fn get_indexes(positions: &IndexMap<usize, Place>, transitions: &IndexMap<usize, Transition>) -> HashMap<VertexIndex, usize> {
        transitions
            .keys()
            .map(|k| VertexIndex::transition(*k))
            .chain(positions.keys().map(|k| VertexIndex::place(*k)))
            .enumerate()
            .fold(HashMap::new(), |mut acc, (index, element)| {
                acc.insert(element, index);
                acc
            })
    }

    fn pre_post_arrays(net: &PetriNet) -> (HashMap<VertexIndex, Vec<VertexIndex>>, HashMap<VertexIndex, Vec<VertexIndex>>) {
        let mut pre = HashMap::<VertexIndex, Vec<VertexIndex>>::new();
        let mut post = HashMap::<VertexIndex, Vec<VertexIndex>>::new();
        for connection in net.directed().iter() {
            pre.entry(connection.end())
                .or_insert_with(Vec::new)
                .push(connection.begin());

            post.entry(connection.begin().clone())
                .or_insert_with(Vec::new)
                .push(connection.end());
        }

        (pre, post)
    }

    pub fn get_loops(net: &PetriNet) -> Vec<Vec<VertexIndex>> {
        let mut loops = Vec::<Vec<VertexIndex>>::new();

        let (positions, transitions) = (&net.places(), &net.transitions());
        let indexes = get_indexes(&positions, &transitions);
        let (pre, post) = pre_post_arrays(net);

        let mut b = Vec::new();
        b.resize(indexes.len(), Default::default());
        for index in indexes.iter() {
            b[*index.1] = index.0.clone();
        }

        let mut a_static = nalgebra::DMatrix::<u64>::zeros(b.len(), b.len());
        for (element, children) in pre.iter() {
            let Some(&element_index) = indexes.get(element) else { continue };
            for child in children.iter() {
                let Some(&child_index) = indexes.get(child) else { continue };
                match element.type_ {
                    VertexType::Place => a_static.row_mut(element_index)[child_index] = 1,
                    VertexType::Transition => a_static.column_mut(child_index)[element_index] = 1,
                }
            }
        }

        let mut ad = a_static.clone();
        for i in 2..=b.len() {
            ad *= a_static.clone();
            let ad_dig = ad.diagonal();

            'cont: for (k, _) in ad_dig.iter().enumerate().filter(|(_, e)| **e > 0) {
                let mut x = Vec::new();
                x.resize(i + 1, Vec::new());

                let mut y = Vec::new();
                y.resize(i + 1, Vec::new());

                let mut z = Vec::new();
                z.resize(i + 1, Vec::new());

                x[0] = vec![b[k].clone()];
                y[i] = vec![b[k].clone()];

                let mut j = 1;
                let mut h = i - 1;

                while j <= i {
                    let mut tmp = Vec::new();
                    for el in x[j - 1].iter() {
                        if let Some(v) = post.get(el) {
                            tmp.extend(v.iter().cloned());
                        }
                    }

                    x[j] = tmp;

                    j += 1;

                    let mut tmp = Vec::new();

                    for el in y[h + 1].iter() {
                        if let Some(v) = pre.get(el) {
                            tmp.extend(v.iter().cloned());
                        }
                    }

                    y[h] = tmp;

                    if h != 0 {
                        h -= 1
                    };
                }

                if !x[i].contains(&b[k]) || !y[0].contains(&b[k]) {
                    continue;
                }

                for m in (0..=i).into_iter() {
                    let a = x[m].iter().cloned().collect::<HashSet<_>>();
                    let b = y[m].iter().cloned().collect::<HashSet<_>>();
                    let inter = a.intersection(&b).cloned().collect::<HashSet<_>>();
                    z[m] = x[m]
                        .iter()
                        .filter(|e| inter.contains(*e))
                        .cloned()
                        .collect();
                }

                let d = &z[0];
                let mut v = vec![d.iter().next().unwrap().clone()];
                let mut d = v[0].clone();

                for m in (1..i).into_iter() {
                    let a = post
                        .get(&d)
                        .unwrap()
                        .iter()
                        .cloned()
                        .collect::<HashSet<_>>();
                    let b = z[m].iter().cloned().collect::<HashSet<_>>();
                    let inter = a.intersection(&b).cloned().collect::<HashSet<_>>();

                    let intersection = z[m]
                        .iter()
                        .filter(|e| inter.contains(*e))
                        .cloned()
                        .collect::<Vec<_>>();

                    // if v.contains(&intersection[0]) {
                    //     continue 'cont;
                    // }
                    v.push(intersection[0].clone());
                    d = v.last().unwrap().clone();
                }

                loops.push(v.clone());
                // for part in 2..(i / 2) {
                //     let wind = i / part;
                //
                //     let s1 = v.iter().take(wind).cloned().collect::<Vec<_>>();
                //     let s2 = v.iter().skip(wind).take(wind).cloned().collect::<Vec<_>>();
                //
                //     if s1.iter().zip(s2.iter()).filter(|(a, b)| **a == **b).count() == s1.len() {
                //         *loops.last_mut().unwrap() = s1.to_vec();
                //         break;
                //     }
                // }
            }
        }

        loops.sort_by(|a, b| b.len().cmp(&a.len()));
        loops
    }

    #[test]
    fn simple_test() {
        /*
        ┌──────────────────────────────┐                  
        │                              │                  
        │                              │                  
        │                              │                  
        │                              │                  
        ▼                              │                  
        p1────────►t1───────►p2──────►t2                  
        ▲           │                                     
        │           │                                     
        │           │                                     
        │           └───────►p3──────►t3──────►p4──────►t4
        │                                               │ 
        │                                               │ 
        └───────────────────────────────────────────────┘ 
         */
        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::default()));
        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let p3 = VertexIndex::place(net.add_place(Place::default()));
        let p4 = VertexIndex::place(net.add_place(Place::default()));
        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t2 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t3 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t4 = VertexIndex::transition(net.add_transition(Transition::default()));

        net.add_directed(DirectedEdge::new(p1, t1));
        net.add_directed(DirectedEdge::new(t1, p2));
        net.add_directed(DirectedEdge::new(p2, t2));
        net.add_directed(DirectedEdge::new(t2, p1));
        net.add_directed(DirectedEdge::new(t1, p3));
        net.add_directed(DirectedEdge::new(p3, t3));
        net.add_directed(DirectedEdge::new(t3, p4));
        net.add_directed(DirectedEdge::new(p4, t4));
        net.add_directed(DirectedEdge::new(t4, p1));

        let components = johnson_circuit(&net);
        assert_eq!(
            components.into_iter().collect::<HashSet<_>>(),
            HashSet::from([
                vec![p1, t1, p2, t2],
                vec![p1, t1, p3, t3, p4, t4],
            ])
        )
    }

    #[test]
    fn loop_in_loop() {
        /*
        ┌──────────────────────────────┐                  
        │                              │                  
        │                              │                  
        │                              │                  
        │                              │                  
        ▼                              │                  
        p1────────►t1───────►p2──────►t2                  
        ▲           │                                     
        │           │                                     
        │           │                                     
        │           └───────►p3──────►t3──────►p4──────►t4
        │                              │                │ 
        │                              │                │ 
        └──────────────────────────────┴────────────────┘ 
         */
        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::default()));
        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let p3 = VertexIndex::place(net.add_place(Place::default()));
        let p4 = VertexIndex::place(net.add_place(Place::default()));
        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t2 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t3 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t4 = VertexIndex::transition(net.add_transition(Transition::default()));

        net.add_directed(DirectedEdge::new(p1, t1));
        net.add_directed(DirectedEdge::new(t1, p2));
        net.add_directed(DirectedEdge::new(p2, t2));
        net.add_directed(DirectedEdge::new(t2, p1));
        net.add_directed(DirectedEdge::new(t1, p3));
        net.add_directed(DirectedEdge::new(p3, t3));
        net.add_directed(DirectedEdge::new(t3, p4));
        net.add_directed(DirectedEdge::new(p4, t4));
        net.add_directed(DirectedEdge::new(t4, p1));
        net.add_directed(DirectedEdge::new(t3, p1));

        let components = johnson_circuit(&net);
        assert_eq!(
            components.into_iter().collect::<HashSet<_>>(),
            HashSet::from([
                vec![p1, t1, p2, t2],
                vec![p1, t1, p3, t3],
                vec![p1, t1, p3, t3, p4, t4],
            ])
        )
    }

    #[test]
    fn three_loops() {
        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::default()));
        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let p3 = VertexIndex::place(net.add_place(Place::default()));
        let p4 = VertexIndex::place(net.add_place(Place::default()));
        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t2 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t3 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t4 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t5 = VertexIndex::transition(net.add_transition(Transition::default()));

        net.add_directed(DirectedEdge::new(p1, t1));
        net.add_directed(DirectedEdge::new(p1, t3));
        net.add_directed(DirectedEdge::new(t2, p1));
        net.add_directed(DirectedEdge::new(t3, p2));
        net.add_directed(DirectedEdge::new(t1, p2));
        net.add_directed(DirectedEdge::new(t1, p3));
        net.add_directed(DirectedEdge::new(p2, t2));
        net.add_directed(DirectedEdge::new(p3, t5));
        net.add_directed(DirectedEdge::new(t5, p4));
        net.add_directed(DirectedEdge::new(p4, t4));
        net.add_directed(DirectedEdge::new(t4, p2));

        let components = johnson_circuit(&net);
        for component in components {
            for element in component {
                print!("{},", element)
            }
            println!();
        }

        println!("**********");

        let components = get_loops(&net);
        for component in components {
            for element in component {
                print!("{},", element)
            }
            println!();
        }
        //println!("{:?}", components.into_iter().collect::<HashSet<_>>());
        // assert_eq!(
        //     components.into_iter().collect::<HashSet<_>>(),
        //     HashSet::from([
        //         vec![p1, t1, p2, t2],
        //         vec![p1, t1, p3, t3],
        //         vec![p1, t1, p3, t3, p4, t4],
        //     ])
        // )
    }
}
