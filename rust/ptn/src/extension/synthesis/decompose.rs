/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use std::collections::HashMap;
use crate::core::SetPartitionMesh;
use crate::core::{johnson_circuit::johnson_circuit, MatrixExt, NetPaths};
use crate::net::place::Place;
use crate::net::vertex::{VertexIndex, VertexType};
use crate::net::DirectedEdge;
use crate::net::PetriNet;
use nalgebra::base::DMatrix;
use std::ops::{AddAssign, MulAssign};
use tracing::{debug, error, info, warn};
use good_lp::*;
use good_lp::{microlp};
use good_lp::variable::UnsolvedProblem;

/// Decomposition context
///
/// Contains information on the division of a Petri net into its constituent components
/// and a Petri net in a primitive coordinate system

pub struct DecomposeContextBuilder {
    pub parts: PetriNet,
}

impl DecomposeContextBuilder {
    pub fn new(parts: PetriNet) -> Self {
        DecomposeContextBuilder { parts }
    }

    fn solve_common(a: DMatrix<f64>, b: DMatrix<f64>) -> DMatrix<f64> {
        let mut variables = vec![];

        let mut vars = variables! {};
        for j in 0..a.ncols() {
            let variable = good_lp::variable().name(format!("x_{}", j)).min(0);
            let v = vars.add(variable);

            variables.push(v);
        }

        let mut constraints = vec![];
        for i in 0..a.nrows() {
            let mut var_vec = vec![];
            for j in 0..a.ncols() {
                if a[(i, j)] > 0. {
                    var_vec.push((true, j));
                } else if a[(i, j)] < 0. {
                    var_vec.push((false, j)); // neg
                }
            }

            if var_vec.len() == 2 {
                let (var0_pos, var0) = var_vec[0];
                let (var1_pos, var1) = var_vec[1];

                let sol = b[i] as i32;
                if var0_pos && !var1_pos {
                    constraints.push(constraint!(variables[var0] - variables[var1] == sol));
                } else if !var0_pos && var1_pos {
                    constraints.push(constraint!(variables[var1] - variables[var0] == sol));
                } else if var0_pos && var1_pos {
                    constraints.push(constraint!(variables[var0] + variables[var1] == sol));
                } else {
                    constraints.push(constraint!(-1 * variables[var0] - variables[var1] == sol));
                }
            } else if var_vec.len() > 2 {
                let sol = b[i] as i32;
                let mut expr = variables[var_vec[0].1] + variables[var_vec[1].1];
                for &(_, j) in var_vec.iter().skip(2) {
                    expr += variables[j];
                }

                constraints.insert(0, constraint!(expr == sol));
            }
        }

        info!("vars => {:#?}", constraints);

        let mut solution = vars.maximise(1).using(microlp).with_all(constraints).solve().expect("solution");
        let mut array1 = DMatrix::zeros(a.nrows(), 1);
        for (i, v) in variables.into_iter().enumerate() {
            array1[(i, 0)] = solution.value(v);
        }

        info!("solution => {:#?}", array1);
        array1
    }
    
    // A[nxn] * B[nx1] = C[nx1]
    fn solve_with_mu(mut a: DMatrix<f64>, mut b: DMatrix<f64>) -> DMatrix<f64> {
        // find all negative in n/2 equation
        let mu_equation_index = a.nrows() / 2;
        for i in 0..mu_equation_index {
            for j in 0..a.ncols() {
                if a[(i, j)] == -1. && a[(mu_equation_index, j)] != 0. {
                    let multiplier = a[(mu_equation_index, j)];
                    a.row_mut(i).mul_assign(multiplier);
                    b[(i, 0)] *= multiplier;
        
                    let a_row = a.row(i).into_owned();
                    let mut mu_row = a.row_mut(mu_equation_index);
                    mu_row.add_assign(&a_row);
        
                    b[(mu_equation_index, 0)] += b[i];
                }
            }
        }
        
        // Set free variable
        let mut free_variable = 0;
        for j in 0..a.ncols() {
            if a[(mu_equation_index, j)] > 0. {
                free_variable = j;
                break;
            }
        }
        
        // Set other negative to zero
        let mut next_equation_index = mu_equation_index + 1;
        info!("{}", next_equation_index);
        'outer: for i in 0..mu_equation_index {
            if next_equation_index >= a.nrows() {
                break 'outer;
            }
        
            for j in 0..a.ncols() {
                if a[(i, j)] > 0. && j == free_variable {
                    continue 'outer;
                }
            }
        
            for j in 0..a.ncols() {
                if b[i] > 0. && a[(i, j)] > 0. {
                    a[(next_equation_index, j)] = a[(i, j)];
                    b[(next_equation_index, 0)] = b[i];
                    next_equation_index += 1;
                    break;
                } else if b[i] < 0. && a[(i, j)] < 0. {
                    a[(next_equation_index, j)] = a[(i, j)].abs();
                    b[(next_equation_index, 0)] -= b[i];
                    next_equation_index += 1;
                    break;
                } else if a[(i, j)] < 0. {
                    a[(next_equation_index, j)] = a[(i, j)].abs();
                    next_equation_index += 1;
                    break;
                }
            }
        }

        Self::solve_common(a, b)
    }

    fn solve_without_mu(mut a: DMatrix<f64>, mut b: DMatrix<f64>) -> DMatrix<f64> {
        let mut mu_equation_index = a.nrows() / 2;
        for i in 0..mu_equation_index {
            for j in 0..a.ncols() {
                if b[i] > 0. && a[(i, j)] == 1. {
                    a[(mu_equation_index, j)] = 1.;
                    b[(mu_equation_index, 0)] = b[i];
                    mu_equation_index += 1;
                    break;
                } else if b[i] < 0. && a[(i, j)] == -1. {
                    a[(mu_equation_index, j)] = 1.;
                    b[(mu_equation_index, 0)] -= b[i];
                    mu_equation_index += 1;
                    break;
                } else if a[(i, j)] == -1. {
                    a[(mu_equation_index, j)] = 1.;
                    mu_equation_index += 1;
                    break;
                }

                // if a[(i, j)] == -1. {
                //     a[(mu_equation_index, j)] = 1.;
                //     mu_equation_index += 1;
                // }
            }
        }

        Self::solve_common(a, b)
    }

    fn solve(a: DMatrix<f64>, b: DMatrix<f64>) -> DMatrix<f64> {
        let mu_equation_index = a.nrows() / 2;
        let mu_equation = a.row(mu_equation_index);
        if mu_equation.iter().any(|&x| x != 0.) {
            DecomposeContextBuilder::solve_with_mu(a, b)
        } else {
            DecomposeContextBuilder::solve_without_mu(a, b)
        }
    }

    pub fn calculate_c_matrix(
        positions: usize,
        transitions: usize,
        linear_base_fragments: &(DMatrix<f64>, DMatrix<f64>),
        primitive_matrix: DMatrix<f64>,
        mu: &DMatrix<f64>,
    ) -> DMatrix<f64> {
        let mut c_matrix = nalgebra::DMatrix::<f64>::zeros(positions, positions);
        let d_matrix = linear_base_fragments.1.clone() - linear_base_fragments.0.clone();

        for row in 0..positions {
            let mut array_a = DMatrix::<f64>::zeros(positions, positions);
            let mut array_b = DMatrix::<f64>::zeros(positions, 1);

            for col in 0..transitions {
                array_a.row_mut(col).copy_from_slice(primitive_matrix.column(col).as_slice());
                array_b[(col, 0)] = d_matrix[(row, col)];
            }

            // set mu equation
            // todo write test
            array_a.row_mut(transitions).copy_from_slice(mu.as_slice());
            array_b[(transitions, 0)] = mu[(0, row)];

            let solution = DecomposeContextBuilder::solve(array_a, array_b);
            c_matrix
                .row_mut(row)
                .copy_from_slice(solution.as_slice());
        }

        debug!("Tensor C: {}", c_matrix);

        c_matrix
    }

    pub fn build(self) -> DecomposeContext {
        let mut parts = self.parts;
        let place_cnt = parts.places().len();
        let transition_cnt = parts.transitions().len();

        let primitive_net = primitive_net(&mut parts);
        let adjacency_primitive = primitive_net.one_zero_adjacency_matrices();

        let linear_base_fragments_matrix = parts.one_zero_adjacency_matrices();
        //let linear_base_fragments_matrix_f64 = parts.one_zero_adjacency_matrices::<f64>();
        let mu = DMatrix::from_row_slice(
            1,
            place_cnt,
            &parts
                .places()
                .iter()
                .map(|(_, place)| place.markers() as f64)
                .collect::<Vec<_>>(),
        );

        debug!(
            "input: {} output: {}",
            adjacency_primitive.0, adjacency_primitive.1
        );
        debug!(
            "input: {} output: {}",
            linear_base_fragments_matrix.0, linear_base_fragments_matrix.1
        );
        let c_matrix = DecomposeContextBuilder::calculate_c_matrix(
            place_cnt,
            transition_cnt,
            &linear_base_fragments_matrix,
            adjacency_primitive.1.clone() - adjacency_primitive.0.clone(),
            &mu,
        );

        DecomposeContext {
            primitive_net,
            linear_base_fragments_net: parts,
            c_matrix,
        }
    }
}

#[derive(Debug, Clone)]
pub struct DecomposeContext {
    pub primitive_net: PetriNet,
    pub linear_base_fragments_net: PetriNet,
    // pub place_set_partitions: SetPartitionMesh,
    // pub transition_set_partitions: SetPartitionMesh,
    pub c_matrix: DMatrix<f64>,
}

impl DecomposeContext {
    pub fn init(net: &PetriNet) -> Self {
        // TODO check connection types
        let mut net = net.clone();
        let mut parts = vec![];

        parts.extend(extract_loops(&mut net));
        parts.extend(extract_paths(&mut net));

        let mut lbf_net = PetriNet::new();
        for part in parts {
            part.places().iter().for_each(|(key, v)| {
                lbf_net.insert_place(*key, v.clone(), None);
            });
            part.transitions().iter().for_each(|(key, v)| {
                lbf_net.insert_transition(*key, v.clone(), None);
            });
            part.directed().iter().for_each(|conn| {
                lbf_net.add_directed(conn.clone());
            });
        }

        normalize_net(&mut lbf_net);

        DecomposeContextBuilder::new(lbf_net).build()
    }

    pub fn marking(&self) -> DMatrix<f64> {
        let mut marking = DMatrix::zeros(self.linear_base_fragments_net.places().len(), 1);
        for (i, (_, p)) in self.linear_base_fragments_net.places().iter().enumerate() {
            marking[(i, 0)] = p.markers() as f64;
        }
        marking
    }

    pub fn linear_base_fragments(&self) -> &PetriNet {
        &self.linear_base_fragments_net
    }

    pub fn program_value(&self, _: usize, _: usize) -> usize {
        //self.programs()[program].data[index] as usize
        0
    }

    pub fn set_program_value(&mut self, _: usize, _: usize, _: usize) {
        //self.programs[program].data[index] = value as u16;
    }

    pub fn program_equation(&self, _index: usize) -> String {
        // let pos_indexes_vec = self.places();
        // let tran_indexes_vec = self.transitions();

        // let program =
        //     SynthesisProgram::new_with(self.programs.get_partition(index), tran_indexes_vec.len());
        //
        // let (t_sets, p_sets) = program.sets();
        //
        // info!(
        //     "{:?}\n{:?}\n{:?}\n{:?}",
        //     t_sets,
        //     p_sets,
        //     pos_indexes_vec,
        //     tran_indexes_vec
        // );
        // TODO:
        let result = String::new();
        // for set in t_sets {
        //     if set.is_empty() {
        //         continue;
        //     }
        //
        //     result += tran_indexes_vec[*set.last().unwrap()].name().as_str();
        //     result += " = ";
        //     for i in (0..set.len()).rev() {
        //         result += &tran_indexes_vec[set[i]].name();
        //         if i > 0 {
        //             result += " + ";
        //         }
        //     }
        //
        //     result += "\n";
        // }
        //
        // for set in p_sets {
        //     if set.is_empty() {
        //         continue;
        //     }
        //
        //     result += &pos_indexes_vec[*set.last().unwrap()].name().as_str();
        //     result += " = ";
        //     for i in (0..set.len()).rev() {
        //         result += &pos_indexes_vec[set[i]].name();
        //         if i > 0 {
        //             result += " + ";
        //         }
        //     }
        //
        //     result += "\n";
        // }

        result
    }

    pub fn program_header_name(&self, _index: usize, _label: bool) -> String {
        // if index < self.transitions().len() {
        //     match label {
        //         true => self.transitions()[index].label(false),
        //         false => self.transitions()[index].full_name(),
        //     }
        // } else {
        //     match label {
        //         true => self.places()[index - self.transitions().len()].label(false),
        //         false => self.places()[index - self.transitions().len()].full_name(),
        //     }
        // }
        // TODO:
        String::new()
    }

    pub fn primitive_net(&self) -> &PetriNet {
        &self.primitive_net
    }

    pub fn transition_synthesis_program(
        &self,
        t_set: &Vec<usize>,
        adjacency_input: &mut DMatrix<f64>,
        adjacency_output: &mut DMatrix<f64>,
    ) {
        assert!(t_set.len() > 1);

        let first = t_set[0];
        for &t in t_set.iter().skip(1) {
            adjacency_input.logical_column_add(first, t);
            adjacency_output.logical_column_add(first, t);
            // logical_column_add(adjacency_input, first, t);
            // logical_column_add(adjacency_output, first, t);
        }

        for &t in t_set.iter().skip(1) {
            adjacency_input.logical_column_add(t, first);
            adjacency_output.logical_column_add(t, first);
        }
    }

    pub fn position_synthesis_program(
        &self,
        p_set: &Vec<usize>,
        adjacency_input: &mut DMatrix<f64>,
        adjacency_output: &mut DMatrix<f64>,
        d_markers: &mut DMatrix<f64>,
    ) {
        let first = p_set[0];
        for p in p_set.iter().skip(1) {
            adjacency_input.logical_row_add(first, *p);
            adjacency_output.logical_row_add(first, *p);
            // logical_row_add(adjacency_input, first, *p);
            // logical_row_add(adjacency_output, first, *p);
            d_markers[(first, 0)] = d_markers[(*p, 0)].max(d_markers[(first, 0)]);
        }

        for p in p_set.iter().skip(1) {
            // logical_row_add(adjacency_input, *p, first);
            // logical_row_add(adjacency_output, *p, first);
            d_markers[(*p, 0)] = d_markers[(first, 0)];
        }
    }
}

/// Normalizes a linear-base fragment so that positions between two transitions are duplicated
fn normalize_net(net: &mut PetriNet) {
    struct Split {
        id: usize,
        place: Place,
        input_transition: usize,
        output_transition: usize,
    }

    let mut splits = vec![];
    for (&id, place) in net.places() {
        // Let's check that the position is not an input or output, i.e. it is located between two transitions
        let v_idx = VertexIndex::place(id);
        let Some(input) = net.directed().iter().find(|conn| conn.end() == v_idx) else {
            continue;
        };
        let Some(output) = net.directed().iter().find(|conn| conn.begin() == v_idx) else {
            continue;
        };

        splits.push(Split {
            id,
            place: place.clone(),
            input_transition: input.begin().id,
            output_transition: output.end().id,
        });
    }

    for split in splits.into_iter() {
        let place = net.add_place(split.place);
        net.move_place(place, split.id);

        net.add_directed(DirectedEdge::new(
            VertexIndex::transition(split.input_transition),
            VertexIndex::place(place),
        ));
        net.add_directed(DirectedEdge::new(
            VertexIndex::place(place),
            VertexIndex::transition(split.output_transition),
        ));
    }
}

/// Removes vertices from a Petri net such that
/// if the node is referenced by elements that are not included
/// in the array of nodes to be deleted, then it is duplicated
///
/// # Important
/// Nodes in the array must alternate T/P or P/T
///
/// # Returns
/// [`PetriNet`] - extracted PN
fn extract_part(net: &mut PetriNet, remove: &[VertexIndex]) -> PetriNet {
    debug!("remove part {remove:?}");
    let mut result = PetriNet::new();

    // Let's add vertices to the new network
    for &index in remove {
        let id = index.id;
        match index.type_ {
            VertexType::Place => {
                result.insert_place(id, net.place(id).expect("unreachable").clone(), None);
            }
            VertexType::Transition => {
                result.insert_transition(
                    id,
                    net.transition(id).expect("unreachable").clone(),
                    None,
                );
            }
        }
    }

    // Connect the added vertices and remove connections from the current network
    for connect in remove.windows(2) {
        assert_ne!(
            connect[0].type_, connect[1].type_,
            "vertices must be different types"
        );
        // delete connection in current network
        let removed = net
            .remove_directed(connect[0], connect[1])
            .expect("BUG: the connection must exists in net");
        result.add_directed(removed);
    }

    // Let's check that this is a cycle: if the types of the 1st element and the last one are not equal, then this is a cycle
    if remove[0].type_ != remove[remove.len() - 1].type_ {
        let removed = net
            .remove_directed(remove[remove.len() - 1], remove[0])
            .expect("BUG: the connection must exists in net");
        result.add_directed(removed)
    }

    // For each element to be removed, we check:
    // if there are connections left, then duplicate the element and connections and then delete
    for &index in remove {
        let found = net
            .directed()
            .iter()
            .find(|conn| conn.begin() == index || conn.end() == index)
            .is_some();

        // If we find that the element has a connection, we split it and then delete the parent element
        if found {
            let split = split_element(net, index);

            // When the element being divided is a transition, we check that it has a connection coming out of it
            // Otherwise, it is necessary to return the position in which the parent element is located and also split it.
            if VertexType::Transition == split.type_ {
                if net
                    .directed()
                    .iter()
                    .find(|conn| conn.begin() == index)
                    .is_none()
                {
                    let connection = result
                        .directed()
                        .iter()
                        .find(|conn| conn.begin() == index)
                        .copied()
                        .expect("an output place must exists");

                    let place_key = net.add_place(
                        result
                            .place(connection.end().id)
                            .cloned()
                            .expect("BUG: place must exists"),
                    );
                    net.add_directed(DirectedEdge::new_with(
                        split,
                        VertexIndex::place(place_key),
                        connection.weight(),
                    ));
                }

                // When the element being divided is a transition, we check that it includes a connection
                // Otherwise, it is necessary to return the position that is included in the parent element and also split it
                if net
                    .directed()
                    .iter()
                    .find(|conn| conn.end() == index)
                    .is_none()
                {
                    let connection = result
                        .directed()
                        .iter()
                        .find(|conn| conn.end() == index)
                        .copied()
                        .expect("an input place must exists");

                    let place_key = net.add_place(
                        result
                            .place(connection.begin().id)
                            .cloned()
                            .expect("BUG: place must exists"),
                    );
                    net.add_directed(DirectedEdge::new_with(
                        VertexIndex::place(place_key),
                        split,
                        connection.weight(),
                    ));
                }
            }
        }

        net.remove(index);
    }

    //println!("{:#?}", net);
    warn!("RESULT: {:#?}", result);
    result
}

// Splits an element in a network into 2 elements, also copies connections
fn split_element(net: &mut PetriNet, index: VertexIndex) -> VertexIndex {
    let id = index.id;
    let new_element_index = match index.type_ {
        VertexType::Place => {
            let element = net
                .place(id)
                .cloned()
                .expect("when this method call, net always contains place by key");
            VertexIndex::place(net.add_place(element))
        }
        VertexType::Transition => {
            let element = net
                .transition(id)
                .cloned()
                .expect("when this method call, net always contains transition by key");
            VertexIndex::transition(net.add_transition(element))
        }
    };

    // Copy connections
    let connections = net
        .directed()
        .iter()
        .filter_map(|conn| {
            if conn.begin() == index {
                Some(DirectedEdge::new_with(
                    new_element_index,
                    conn.end(),
                    conn.weight(),
                ))
            } else if conn.end() == index {
                Some(DirectedEdge::new_with(
                    conn.begin(),
                    new_element_index,
                    conn.weight(),
                ))
            } else {
                None
            }
        })
        .collect::<Vec<_>>();

    for conn in connections {
        net.add_directed(conn);
    }

    new_element_index
}

/// Removes all cycles from a Petri net (starting with the largest one)
fn extract_loops(net: &mut PetriNet) -> Vec<PetriNet> {
    let mut loops = vec![];
    loop {
        let net_loops = johnson_circuit(net);
        match get_longest(&net_loops) {
            None => break,
            Some(l) => loops.push(extract_part(net, l)),
        }
    }
    loops
}

pub fn get_longest(paths: &[Vec<VertexIndex>]) -> Option<&[VertexIndex]> {
    paths
        .iter()
        .max_by(|a, b| a.len().cmp(&b.len()))
        .map(|v| v.as_slice())
}

/// Removes all direct paths from a Petri net (starting with the largest one)
fn extract_paths(net: &mut PetriNet) -> Vec<PetriNet> {
    let mut paths = vec![];
    loop {
        let net_paths = NetPaths::find(net);
        match net_paths.get_longest() {
            None => break,
            Some(l) => paths.push(extract_part(net, l)),
        }
    }
    paths
}

fn primitive_net(net: &mut PetriNet) -> PetriNet {
    let mut result = PetriNet::new();

    let transitions = net.transitions();

    'brk: for (&transition_id, _transition) in transitions {
        let transition_idx = VertexIndex::transition(transition_id);
        let mut from = net
            .directed()
            .iter()
            .filter(|c| c.begin().eq(&transition_idx));

        while let Some(t_to_p) = from.next() {
            if result.place(t_to_p.end().id).is_some() {
                continue;
            }

            let mut to = net
                .directed()
                .iter()
                .filter(|c| c.begin().ne(&t_to_p.end()) && c.end().eq(&transition_idx));

            while let Some(p_to_t) = to.next() {
                if result.place(p_to_t.begin().id).is_some() {
                    continue;
                }

                let transition_key = t_to_p.begin().id;
                result.insert_transition(
                    transition_key,
                    net.transition(transition_key).cloned().unwrap(),
                    None,
                );

                let input_place_key = p_to_t.begin().id;
                result.insert_place(
                    input_place_key,
                    net.place(input_place_key).cloned().unwrap(),
                    None,
                );

                let output_place_key = t_to_p.end().id;
                result.insert_place(
                    output_place_key,
                    net.place(output_place_key).cloned().unwrap(),
                    None,
                );

                result.add_directed(DirectedEdge::new(p_to_t.begin(), p_to_t.end())); // to_t.weight()
                result.add_directed(DirectedEdge::new(t_to_p.begin(), t_to_p.end())); // from_t.weight()
                continue 'brk;
            }
        }
    }

    result
}

#[cfg(test)]
mod tests {
    use crate::extension::synthesis::decompose::{extract_loops, extract_part, extract_paths};
    use crate::net::vertex::VertexIndex;
    use crate::net::{place::Place, transition::Transition};
    use crate::net::{DirectedEdge, PetriNet};

    #[test]
    fn test_extract_path_1() {
        // p1 -> t1 -> p2
        //        \->  p3

        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::default()));
        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        net.add_directed(DirectedEdge::new(p1, t1));

        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let p3 = VertexIndex::place(net.add_place(Place::default()));
        net.add_directed(DirectedEdge::new(t1, p2));
        net.add_directed(DirectedEdge::new(t1, p3));

        let loops = extract_loops(&mut net);
        assert!(loops.is_empty());

        let paths = extract_paths(&mut net);
        //dbg!(&paths);
        assert_eq!(paths.len(), 2);
        
        let path1 = &paths[0];
        assert_eq!(path1.places().len(), 2);
        assert_eq!(path1.transitions().len(), 1);
        assert_eq!(path1.directed().len(), 2);
        assert!(path1.place(p1.id).is_some());
        assert!(path1.transition(t1.id).is_some());
        assert!(path1.place(p2.id).is_some() || path1.place(p3.id).is_some());
    }

    #[test]
    fn test_extract_path_2() {
        // p1 -> t1 -> p2
        //        \->  p3

        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::default()));
        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let p3 = VertexIndex::place(net.add_place(Place::default()));
        let p4 = VertexIndex::place(net.add_place(Place::default()));
        let p5 = VertexIndex::place(net.add_place(Place::default()));
        let p6 = VertexIndex::place(net.add_place(Place::default()));
        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t2 = VertexIndex::transition(net.add_transition(Transition::default()));

        net.add_directed(DirectedEdge::new(p1, t1));
        net.add_directed(DirectedEdge::new(p2, t1));
        net.add_directed(DirectedEdge::new(p3, t1));

        net.add_directed(DirectedEdge::new(t1, p4));
        net.add_directed(DirectedEdge::new(p4, t2));
        net.add_directed(DirectedEdge::new(t2, p5));

        net.add_directed(DirectedEdge::new(t1, p6));

        let loops = extract_loops(&mut net);
        assert!(loops.is_empty());

        let paths = extract_part(&mut net, &[p1, t1, p4, t2, p5]);
        //dbg!(&paths);

        println!("{:#?}", net);
    }
}
