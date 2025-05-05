/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use super::adjacent;
use std::collections::HashMap;
use std::iter::FromIterator;
use tracing::debug;

use crate::net::vertex::VertexIndex;
use crate::net::PetriNet;

#[derive(Debug, PartialEq)]
pub enum NodeMark {
    None,
    Temporary,
    Permanent,
}

#[derive(Debug, Default)]
pub struct NetPaths {
    paths: Vec<Vec<VertexIndex>>,
}

impl NetPaths {
    /// Find all paths from input positions to output positions if exists
    pub fn find(net: &PetriNet) -> NetPaths {
        // get input positions
        let input_positions = net
            .places()
            .iter()
            .filter(|(index, _)| {
                net.directed()
                    .iter()
                    .find(|c| c.end() == VertexIndex::place(**index))
                    .is_none()
            })
            .map(|(index, _)| *index)
            .collect::<Vec<_>>();

        // get output positions
        let output_positions = net
            .places()
            .iter()
            .filter(|(index, _)| {
                net.directed()
                    .iter()
                    .find(|c| c.begin() == VertexIndex::place(**index))
                    .is_none()
            })
            .map(|(index, _)| *index)
            .collect::<Vec<_>>();

        if input_positions.is_empty() || output_positions.is_empty() {
            return NetPaths::default();
        }

        // DFS Top Sort
        let vertices = net
            .places()
            .keys()
            .map(|key| VertexIndex::place(*key))
            .chain(
                net.transitions()
                    .keys()
                    .map(|key| VertexIndex::transition(*key)),
            )
            .map(|v| (v, NodeMark::None));

        let mut visited = HashMap::from_iter(vertices);
        let mut paths = vec![];
        let mut stack = vec![];
        for &vert in input_positions.iter() {
            let vert = VertexIndex::place(vert);
            visited.values_mut().for_each(|m| *m = NodeMark::None);
            if let Err(_) = NetPaths::process_dfs(net, vert, &mut visited, &mut paths, &mut stack) {
                return NetPaths::default();
            }
        }

        NetPaths { paths }
    }

    pub fn process_dfs(
        net: &PetriNet,
        vertex: VertexIndex,
        mut visited: &mut HashMap<VertexIndex, NodeMark>,
        paths: &mut Vec<Vec<VertexIndex>>,
        stack: &mut Vec<VertexIndex>,
    ) -> Result<(), ()> {
        stack.push(vertex.clone());
        visited.insert(vertex, NodeMark::Permanent);

        let adjacent = adjacent(net, vertex);
        if adjacent.is_empty() {
            paths.push(Vec::from_iter(stack.iter().copied()));
        } else {
            for vert in adjacent {
                if visited[&vert] == NodeMark::Permanent {
                    return Err(())
                }
                NetPaths::process_dfs(net, vert, visited, paths, stack)?;
            }
        }

        visited.insert(vertex, NodeMark::None);
        stack.pop();
        Ok(())
    }

    pub fn get_longest(&self) -> Option<&[VertexIndex]> {
        self.paths
            .iter()
            .max_by(|a, b| a.len().cmp(&b.len()))
            .map(|a| a.as_slice())
    }
}

#[cfg(test)]
mod tests {
    use std::collections::HashSet;
    use crate::core::graph::path::NetPaths;
    use crate::net::{DirectedEdge, PetriNet};
    use crate::net::place::Place;
    use crate::net::transition::Transition;
    use crate::net::vertex::VertexIndex;

    #[test]
    fn test_find_path() {
        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::default()));
        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let p3 = VertexIndex::place(net.add_place(Place::default()));
        let p4 = VertexIndex::place(net.add_place(Place::default()));
        let p5 = VertexIndex::place(net.add_place(Place::default()));

        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t2 = VertexIndex::transition(net.add_transition(Transition::default()));

        net.add_directed(DirectedEdge::new(p1, t1));
        net.add_directed(DirectedEdge::new(t1, p2));
        net.add_directed(DirectedEdge::new(p2, t2));
        net.add_directed(DirectedEdge::new(t2, p3));
        net.add_directed(DirectedEdge::new(p4, t2));
        net.add_directed(DirectedEdge::new(t2, p5));

        let paths = NetPaths::find(&net);
        assert_eq!(
            paths.paths.into_iter().collect::<HashSet<_>>(),
            HashSet::from([
                vec![p1, t1, p2, t2, p5],
                vec![p1, t1, p2, t2, p3],
                vec![p4, t2, p3],
                vec![p4, t2, p5]
            ])
        )
    }

    #[test]
    fn test_find_path2() {
        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::default()));
        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let p3 = VertexIndex::place(net.add_place(Place::default()));
        let p4 = VertexIndex::place(net.add_place(Place::default()));
        let p5 = VertexIndex::place(net.add_place(Place::default()));
        let p6 = VertexIndex::place(net.add_place(Place::default()));

        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t2 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t3 = VertexIndex::transition(net.add_transition(Transition::default()));

        net.add_directed(DirectedEdge::new(p1, t1));
        net.add_directed(DirectedEdge::new(t1, p2));
        net.add_directed(DirectedEdge::new(p2, t2));
        net.add_directed(DirectedEdge::new(t2, p3));
        net.add_directed(DirectedEdge::new(p3, t3));
        net.add_directed(DirectedEdge::new(t3, p4));

        net.add_directed(DirectedEdge::new(t1, p6));
        net.add_directed(DirectedEdge::new(p6, t2));
        net.add_directed(DirectedEdge::new(t2, p5));

        let paths = NetPaths::find(&net);
        for path in paths.paths.iter() {
            for element in path.iter() {
                print!("{},", element);
            }
            println!();
        }
        // assert_eq!(
        //     paths.paths.into_iter().collect::<HashSet<_>>(),
        //     HashSet::from([
        //         vec![p1, t1, p2, t2, p5],
        //         vec![p1, t1, p2, t2, p3],
        //         vec![p4, t2, p3],
        //         vec![p4, t2, p5]
        //     ])
        // )
    }
}
