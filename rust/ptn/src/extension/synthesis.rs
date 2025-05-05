/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

mod decompose;

use nalgebra::DMatrix;
pub use self::decompose::*;
use crate::core::RestrictedGrowthString;
use crate::net::vertex::VertexIndex;
use crate::net::{DirectedEdge, PetriNet};
use tracing::debug;

///
pub struct SynthesisProgram {
    places_indexes: Vec<Vec<usize>>,
    transitions_indexes: Vec<Vec<usize>>,
}

impl SynthesisProgram {
    /// Creates SynthesisProgram
    pub fn new_with(places: &[u16], transitions: &[u16]) -> Self {
        let places_indexes = sets_of_duplicate_indexes(places);
        let transitions_indexes = sets_of_duplicate_indexes(transitions);
        SynthesisProgram {
            places_indexes,
            transitions_indexes
        }
    }

    pub fn places_sets(&self) -> &[Vec<usize>] {
        &self.places_indexes
    }

    pub fn transitions_sets(&self) -> &[Vec<usize>] {
        &self.transitions_indexes
    }
}

pub fn synthesis_program(context: &DecomposeContext, program: SynthesisProgram) -> PetriNet {
    let mut place_indexes_vec = context.linear_base_fragments_net.places().clone();
    let mut tran_indexes_vec = context.linear_base_fragments_net.transitions().clone();

    let c_matrix = &context.c_matrix;
    let (mut adjacency_input, mut adjacency_output) =
        context.primitive_net.one_zero_adjacency_matrices().clone();

    let mut markers = context.marking();

    let (t_sets, p_sets) = (program.transitions_sets(), program.places_sets());

    for t_set in t_sets.into_iter() {
        context.transition_synthesis_program(&t_set, &mut adjacency_input, &mut adjacency_output);
    }

    for p_set in p_sets.into_iter() {
        context.position_synthesis_program(
            &p_set,
            &mut adjacency_input,
            &mut adjacency_output,
            &mut markers,
        );
    }

    let mut loops = DMatrix::<f64>::zeros(adjacency_input.nrows(), adjacency_input.ncols());
    // // for i in 0..adjacency_input.nrows() {
    // //     for j in 0..adjacency_input.ncols() {
    // //         if adjacency_input[(i, j)] > 0. && adjacency_output[(i, j)] > 0.  {
    // //             loops[(i, j)] = 1.;
    // //          }
    // //     }
    // // }

    adjacency_input = c_matrix * adjacency_input;
    adjacency_output = c_matrix * adjacency_output;
    loops = c_matrix * loops;

    markers = c_matrix * markers;

    let mut adjacency = adjacency_output - adjacency_input;

    let mut remove_rows = vec![];
    for (index, (row_a, row_b)) in adjacency
        .row_iter()
        .zip(loops.row_iter())
        .enumerate()
    {
        if row_a.iter().chain(row_b.iter()).all(|&e| e == 0.) {
            remove_rows.push(index);
            continue;
        }

        for (sub_index, (sub_a, sub_b)) in adjacency
            .row_iter()
            .zip(loops.row_iter())
            .enumerate()
            .skip(index + 1)
        {
            if row_a == sub_a && row_b == sub_b && markers.row(index) == markers.row(sub_index) {
                remove_rows.push(sub_index);
                markers.row_mut(index)[0] =
                    f64::max(markers.row_mut(index)[0], markers.row_mut(sub_index)[0]);
            }
        }
    }

    adjacency = adjacency.remove_rows_at(&remove_rows);
    loops = loops.remove_rows_at(&remove_rows);
    markers = markers.remove_rows_at(&remove_rows);
    place_indexes_vec = place_indexes_vec
        .into_iter()
        .enumerate()
        .filter(|i| !remove_rows.contains(&i.0))
        .map(|(_i, pair)| pair)
        .collect();

    let mut remove_cols = vec![];
    for (index, (col_a, col_b)) in adjacency
        .column_iter()
        .zip(loops.column_iter())
        .enumerate()
    {
        if col_a.iter().chain(col_b.iter()).all(|&e| e == 0.) {
            remove_cols.push(index);
            continue;
        }
        for (sub_index, (sub_a, sub_b)) in adjacency
            .column_iter()
            .zip(loops.column_iter())
            .enumerate()
            .skip(index + 1)
        {
            if col_a == sub_a && col_b == sub_b {
                remove_cols.push(sub_index);
            }
        }
    }

    adjacency = adjacency.remove_columns_at(&remove_cols);
    loops = loops.remove_columns_at(&remove_cols);
    tran_indexes_vec = tran_indexes_vec
        .into_iter()
        .enumerate()
        .filter(|i| !remove_cols.contains(&i.0))
        .map(|i| i.1)
        .collect();

    let mut new_net = PetriNet::new();
    for (idx, (key, place)) in place_indexes_vec.iter().enumerate() {
        let mut place = place.clone();
        debug!("SET MARKERS: {} <= {}", idx, markers.row(idx)[0]);
        place.set_markers(markers.row(idx)[0] as usize);
        new_net.insert_place(*key, place, None);
    }

    for (_idx, (key, transition)) in tran_indexes_vec.iter().enumerate() {
        let transition = transition.clone();
        new_net.insert_transition(*key, transition, None);
    }

    // restore connections
    for row in 0..adjacency.nrows() {
        for col in 0..adjacency.ncols() {
            let place_idx = VertexIndex::place(*place_indexes_vec.get_index(row).unwrap().0);
            let transition_idx =
                VertexIndex::transition(*tran_indexes_vec.get_index(col).unwrap().0);

            let input_weight = loops[(row, col)] as i32;
            if loops[(row, col)] != 0. {
                new_net.add_directed(DirectedEdge::new_with(
                    place_idx,
                    transition_idx,
                    input_weight as u32,
                ));
                new_net.add_directed(DirectedEdge::new_with(
                    transition_idx,
                    place_idx,
                    input_weight as u32,
                ));
                continue
            }

            let input_weight = adjacency[(row, col)] as i32;
            if input_weight > 0 {
                new_net.add_directed(DirectedEdge::new_with(
                    place_idx,
                    transition_idx,
                    input_weight as u32,
                ));
            } else if input_weight < 0 {
                new_net.add_directed(DirectedEdge::new_with(
                    transition_idx,
                    place_idx,
                    input_weight.abs() as u32,
                ));
            }
        }
    }

    new_net
}

fn sets_of_duplicate_indexes(slice: &[u16]) -> Vec<Vec<usize>> {
    let mut set = vec![vec![]; slice.len()];
    for (idx, &search_number) in slice.iter().enumerate() {
        set[search_number as usize].push(idx);
    }
    set.retain(|array| array.len() > 1);
    set
}

#[cfg(test)]
mod tests {
    use crate::core::RestrictedGrowthString;
    use crate::extension::synthesis::{synthesis_program, DecomposeContext, SynthesisProgram};
    use crate::net::{DirectedEdge, PetriNet};
    use crate::net::place::Place;
    use crate::net::transition::Transition;
    use crate::net::vertex::VertexIndex;

    impl Place {
        pub fn with_markers(markers: usize) -> Self {
            let mut place = Place::default();
            place.set_markers(markers as usize);
            place
        }
    }

    #[test]
    fn test_synthesis_program() {
        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::with_markers(1)));
        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let p3 = VertexIndex::place(net.add_place(Place::default()));
        let p4 = VertexIndex::place(net.add_place(Place::default()));

        let p5 = VertexIndex::place(net.add_place(Place::with_markers(1)));
        let p6 = VertexIndex::place(net.add_place(Place::default()));
        let p7 = VertexIndex::place(net.add_place(Place::default()));
        let p8 = VertexIndex::place(net.add_place(Place::default()));

        let p9 = VertexIndex::place(net.add_place(Place::with_markers(1)));
        let p10 = VertexIndex::place(net.add_place(Place::default()));
        let p11 = VertexIndex::place(net.add_place(Place::default()));
        let p12 = VertexIndex::place(net.add_place(Place::default()));

        let p13 = VertexIndex::place(net.add_place(Place::with_markers(1)));
        let p14 = VertexIndex::place(net.add_place(Place::with_markers(1)));
        let p15 = VertexIndex::place(net.add_place(Place::with_markers(1)));

        let p16 = VertexIndex::place(net.add_place(Place::default()));
        let p17 = VertexIndex::place(net.add_place(Place::default()));
        let p18 = VertexIndex::place(net.add_place(Place::default()));
        let p19 = VertexIndex::place(net.add_place(Place::default()));
        let p20 = VertexIndex::place(net.add_place(Place::default()));
        let p21 = VertexIndex::place(net.add_place(Place::default()));
        let p22 = VertexIndex::place(net.add_place(Place::default()));
        let p23 = VertexIndex::place(net.add_place(Place::default()));
        let p24 = VertexIndex::place(net.add_place(Place::default()));

        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t2 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t3 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t4 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t5 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t6 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t7 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t8 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t9 = VertexIndex::transition(net.add_transition(Transition::default()));

        net.add_directed(DirectedEdge::new_with(p1, t1, 1));
        net.add_directed(DirectedEdge::new_with(t1, p2, 1));
        net.add_directed(DirectedEdge::new_with(p2, t2, 1));
        net.add_directed(DirectedEdge::new_with(t2, p3, 1));
        net.add_directed(DirectedEdge::new_with(p3, t3, 1));
        net.add_directed(DirectedEdge::new_with(t3, p4, 1));

        net.add_directed(DirectedEdge::new_with(p5, t4, 1));
        net.add_directed(DirectedEdge::new_with(t4, p6, 1));
        net.add_directed(DirectedEdge::new_with(p6, t5, 1));
        net.add_directed(DirectedEdge::new_with(t5, p7, 1));
        net.add_directed(DirectedEdge::new_with(p7, t6, 1));
        net.add_directed(DirectedEdge::new_with(t6, p8, 1));

        net.add_directed(DirectedEdge::new_with(p9, t7, 1));
        net.add_directed(DirectedEdge::new_with(t7, p10, 1));
        net.add_directed(DirectedEdge::new_with(p10, t8, 1));
        net.add_directed(DirectedEdge::new_with(t8, p11, 1));
        net.add_directed(DirectedEdge::new_with(p11, t9, 1));
        net.add_directed(DirectedEdge::new_with(t9, p12, 1));


        net.add_directed(DirectedEdge::new_with(p13, t1, 1));
        net.add_directed(DirectedEdge::new_with(t1, p16, 1));
        net.add_directed(DirectedEdge::new_with(p16, t4, 1));
        net.add_directed(DirectedEdge::new_with(t4, p19, 1));
        net.add_directed(DirectedEdge::new_with(p19, t7, 1));
        net.add_directed(DirectedEdge::new_with(t7, p22, 1));

        net.add_directed(DirectedEdge::new_with(p14, t2, 1));
        net.add_directed(DirectedEdge::new_with(t2, p17, 1));
        net.add_directed(DirectedEdge::new_with(p17, t5, 1));
        net.add_directed(DirectedEdge::new_with(t5, p20, 1));
        net.add_directed(DirectedEdge::new_with(p20, t8, 1));
        net.add_directed(DirectedEdge::new_with(t8, p23, 1));

        net.add_directed(DirectedEdge::new_with(p15, t3, 1));
        net.add_directed(DirectedEdge::new_with(t3, p18, 1));
        net.add_directed(DirectedEdge::new_with(p18, t6, 1));
        net.add_directed(DirectedEdge::new_with(t6, p21, 1));
        net.add_directed(DirectedEdge::new_with(p21, t9, 1));
        net.add_directed(DirectedEdge::new_with(t9, p24, 1));

        let context = DecomposeContext::init(&net);

        let unite_transitions = &[
            0u16, // t1
            0,  // t2
            0,  // t3
            0,  // t4
            0,  // t5
            0,  // t6
            0,  // t7
            0,  // t8
            0,  // t9
            0,  // t10
            0, // t11
            0, // t12
            0, // t13
            0, // t14
            0, // t15
            0, // t16
            0, // t17
            0, // t18
        ];

        let unite_places = &[
            0u16, //p1
            1, //p2
            2, //p3
            3, //p4
            4, //p5
            5, //p6
            6, //p7
            7, //p8
            8, //p9
            9, //p10
            10, //p11
            11, //p12
            12, //p13
            13, //p14
            14, //p15
            15, //p16
            16, //p17
            17, //p18
            18, //p19
            19, //p20
            20, //p21
            21, //p22
            22, //p23
            23, //p24
            24, //p25
            25, //p26
            26, //p27
            27, //p28
            28, //p29
            29, //p30
            30, //p31
            31, //p32
            32, //p33
            33, //p34
            34, //p35
            35, //p36
        ];
        let program = SynthesisProgram::new_with(unite_places, unite_transitions);
        let net = synthesis_program(&context, program);

        println!("{}", net);

        // let place = RestrictedGrowthString::try_from(vec![0, 1, 2, 3, 4, 5, 6, 7]).unwrap();
        // let transitions = RestrictedGrowthString::try_from(vec![0, 1, 2, 3]).unwrap();
        // let program = SynthesisProgram::new_with(place, transitions);
        // let pos_indexes = vec![
        //     VertexIndex::place(1),
        //     VertexIndex::place(2),
        //     VertexIndex::place(3),
        //     VertexIndex::place(4),
        //     VertexIndex::place(5),
        //     VertexIndex::place(6),
        //     VertexIndex::place(7),
        //     VertexIndex::place(8),
        // ];
        //
        // let tran_indexes = vec![
        //     VertexIndex::transition(1),
        //     VertexIndex::transition(2),
        //     VertexIndex::transition(3),
        //     VertexIndex::transition(4),
        // ];
        //
        // assert!(program.places_sets().is_empty());
        // assert!(program.transitions_sets().is_empty());
        // // TODO: {}
        // // dbg!(program.sets());
    }
}
