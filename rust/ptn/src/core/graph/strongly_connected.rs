/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use nalgebra::min;

#[derive(Debug)]
pub struct StronglyConnected {}

impl StronglyConnected {
    pub fn components(edges: &[Vec<usize>]) -> Vec<Vec<usize>> {
        let mut vertices = (0..edges.len())
            .map(|i| Vertex {
                i,
                index: None,
                low_link: None,
                on_stack: false,
            })
            .collect::<Vec<_>>();

        let mut stack = vec![];
        let mut index = 0;
        let mut components = vec![];

        for v_index in 0..vertices.len() {
            if vertices[v_index].index.is_none() {
                Self::strong_connect(
                    v_index,
                    &mut vertices,
                    &edges,
                    &mut stack,
                    &mut index,
                    &mut components,
                )
            }
        }

        components
    }

    fn strong_connect(
        v_index: usize,
        indexes: &mut [Vertex],
        edges: &[Vec<usize>],
        stack: &mut Vec<usize>,
        index: &mut usize,
        components: &mut Vec<Vec<usize>>,
    ) {
        indexes[v_index].index = Some(*index);
        indexes[v_index].low_link = Some(*index);
        *index += 1;
        stack.push(v_index);
        indexes[v_index].on_stack = true;

        for &w_index in edges[v_index].iter() {
            if indexes[w_index].index.is_none() {
                Self::strong_connect(w_index, indexes, edges, stack, index, components);
                indexes[v_index].low_link =
                    min(indexes[v_index].low_link, indexes[w_index].low_link);
            } else if indexes[w_index].on_stack {
                indexes[v_index].low_link = min(indexes[v_index].low_link, indexes[w_index].index);
            }
        }

        if indexes[v_index].low_link == indexes[v_index].index {
            let mut component = vec![];
            loop {
                let last = stack.pop().expect("BUG");
                indexes[last].on_stack = false;
                component.push(indexes[last].i);

                if indexes[last].i == indexes[v_index].i {
                    break;
                }
            }
            components.push(component)
        }
    }
}

#[derive(Debug, Copy, Clone)]
struct Vertex {
    i: usize,
    index: Option<usize>,
    low_link: Option<usize>,
    on_stack: bool,
}
