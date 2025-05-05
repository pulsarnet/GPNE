/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use crate::net::PetriNet;
use nalgebra::DMatrix;

pub struct PrimitiveDecomposition {
    net: PetriNet,
    primitive_net: PetriNet,
    input_tensor: DMatrix<i32>,
    output_tensor: DMatrix<i32>,
    inverse_input_tensor: DMatrix<i32>,
    inverse_output_tensor: DMatrix<i32>,
}

impl PrimitiveDecomposition {
    pub fn new(mut net: PetriNet) -> Result<Self, ()> {
        Ok(Self {
            net,
            primitive_net: PetriNet::new(),
            input_tensor: DMatrix::<i32>::zeros(1, 1),
            output_tensor: DMatrix::<i32>::zeros(1, 1),
            inverse_input_tensor: DMatrix::<i32>::zeros(1, 1),
            inverse_output_tensor: DMatrix::<i32>::zeros(1, 1),
        })
    }
}

fn khun_algorithnm(
    graph: &Vec<Vec<usize>>,
    mt: &mut Vec<i32>,
    used: &mut Vec<bool>,
    vertex: usize,
) -> bool {
    if used[vertex] {
        return false;
    }

    used[vertex] = true;
    for i in 0..graph[vertex].len() {
        let to = graph[vertex][i];
        if mt[to] == -1 || khun_algorithnm(graph, mt, used, mt[to] as usize) {
            mt[to] = vertex as i32;
            return true;
        }
    }

    return false;
}

fn simplify_matrix(matrix: DMatrix<i32>) -> Option<DMatrix<i32>> {
    let mut candidate_rows = vec![Vec::new(); matrix.nrows()];
    for row in 0..matrix.nrows() {
        for col in 0..matrix.ncols() {
            if matrix[(row, col)] == 1 {
                candidate_rows[row].push(2 * col + 1)
            } else if matrix[(row, col)] == -1 {
                candidate_rows[row].push(2 * col)
            }
        }
    }
    
    // Khun solution
    let mut mt = vec![-1; matrix.nrows()];
    let mut used = vec![false; matrix.nrows()];
    for i in 0..matrix.nrows() {
        used.fill(false);
        khun_algorithnm(&candidate_rows, &mut mt, &mut used, i);
    }
    
    if mt.iter().any(|&x| x == -1) {
        return None;
    }

    // i = 2 * j + 1 => j = (i - 1) / 2
    // i = 2 * j => j = i / 2
    let mut result_matrix = DMatrix::zeros(matrix.nrows(), matrix.ncols());
    for (index, row) in mt.iter().enumerate() {
        if index % 2 == 0 {
            result_matrix[(*row as usize, index / 2)] = -1;
        } else {
            result_matrix[(*row as usize, (index - 1) / 2)] = 1;
        }
    }

    Some(result_matrix)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_simplify_matrix() {
        let matrix = DMatrix::from_row_slice(
            6,
            3,
            &[-1, 0, 0, 1, 1, 1, 1, -1, 0, 0, -1, -1, 0, 1, 0, 0, 0, 1],
        );

        let result = simplify_matrix(matrix);
        println!("{}", result.clone().unwrap());
        assert!(result.is_some());
    }
}
