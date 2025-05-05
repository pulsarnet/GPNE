/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use crate::core::RestrictedGrowthString;
use nalgebra::DMatrix;

// Set Partition Mesh https://ajc.maths.uq.edu.au/pdf/65/ajc_v65_p152.pdf
#[derive(Debug, Clone)]
pub struct SetPartitionMesh {
    triangle: DMatrix<usize>,
    max: usize,
}

impl SetPartitionMesh {
    pub fn new(n: usize) -> SetPartitionMesh {
        let triangle = make_mesh(n);
        let max = triangle.get((0, 0)).copied().unwrap_or(0);

        SetPartitionMesh {
            triangle,
            max,
        }
    }

    pub fn max(&self) -> usize {
        self.max
    }

    pub fn len(&self) -> usize {
        self.triangle.nrows()
    }

    // Traversal of a Set Partition Mesh
    pub fn get_rgs(&self, mut index: usize) -> RestrictedGrowthString {
        if index >= self.max {
            panic!("Index out of bounds");
        }

        let n = self.triangle.nrows();
        let mut rgs = vec![0; n];
        let (mut i, mut j) = (1, 0);

        while i < n {
            let factor = std::cmp::min(index / self.triangle[(i, j)], j + 1);
            rgs[i] = factor as u16;
            index -= factor * self.triangle[(i, j)];
            if factor == (j + 1) {
                j += 1;
            }
            i += 1;
        }

        // SAFETY: rgs is a valid restricted growth string
        RestrictedGrowthString::try_from(rgs).unwrap()

        // rev index
        //let index = self.max - index - 1;

        // first is positions, second is transitions
        // let position_index = index / self.transitions[(0, 0)];
        // let transition_index = index % self.transitions[(0, 0)];
    }

    pub fn get_index(&self, rgs: &RestrictedGrowthString) -> usize {
        let n = self.triangle.nrows();
        let mut index = 0;
        let (mut i, mut j) = (1, 0);

        while i < n {
            let factor = rgs[i] as usize;
            index += factor * self.triangle[(i, j)];
            if factor == (j + 1) {
                j += 1;
            }
            i += 1;
        }

        index
        // let position_index = get_index_by_partition(&self.places, &partition[..self.places.nrows()]);
        // let transition_index = get_index_by_partition(&self.transitions, &partition[self.places.nrows()..]);
        //
        // transition_index * self.places[(0, 0)] + position_index
    }
}

fn make_mesh(size: usize) -> DMatrix<usize> {
    let mut mesh = DMatrix::zeros(size, size);
    if size == 0 {
        return mesh;
    }
    
    mesh.row_mut(mesh.nrows() - 1).fill(1);

    // Equation from https://ajc.maths.uq.edu.au/pdf/65/ajc_v65_p152.pdf page 163(12)
    for i in (0..(size - 1)).rev() {
        for j in 0..=i {
            mesh[(i, j)] = (j + 1) * mesh[(i + 1, j)] + mesh[(i + 1, j + 1)];
        }
    }

    mesh
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_mesh() {
        let mesh = SetPartitionMesh::new(4);
        assert_eq!(mesh.max, 15);
        assert_eq!(
            mesh.triangle,
            DMatrix::from_row_slice(4, 4, &[15, 0, 0, 0, 5, 10, 0, 0, 2, 3, 4, 0, 1, 1, 1, 1,])
        );

        assert_eq!(mesh.get_rgs(0).data(), &[0, 0, 0, 0]);
        assert_eq!(mesh.get_rgs(1).data(), &[0, 0, 0, 1]);
        assert_eq!(mesh.get_rgs(2).data(), &[0, 0, 1, 0]);
        assert_eq!(mesh.get_rgs(3).data(), &[0, 0, 1, 1]);
        assert_eq!(mesh.get_rgs(4).data(), &[0, 0, 1, 2]);
        assert_eq!(mesh.get_rgs(5).data(), &[0, 1, 0, 0]);
        assert_eq!(mesh.get_rgs(6).data(), &[0, 1, 0, 1]);
        assert_eq!(mesh.get_rgs(7).data(), &[0, 1, 0, 2]);
        assert_eq!(mesh.get_rgs(8).data(), &[0, 1, 1, 0]);
        assert_eq!(mesh.get_rgs(9).data(), &[0, 1, 1, 1]);
        assert_eq!(mesh.get_rgs(10).data(), &[0, 1, 1, 2]);
        assert_eq!(mesh.get_rgs(11).data(), &[0, 1, 2, 0]);
        assert_eq!(mesh.get_rgs(12).data(), &[0, 1, 2, 1]);
        assert_eq!(mesh.get_rgs(13).data(), &[0, 1, 2, 2]);
        assert_eq!(mesh.get_rgs(14).data(), &[0, 1, 2, 3]);
    }

    #[test]
    fn test_get_index() {
        let mesh = SetPartitionMesh::new(4);

        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 0, 0, 0]).unwrap()), 0);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 0, 0, 1]).unwrap()), 1);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 0, 1, 0]).unwrap()), 2);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 0, 1, 1]).unwrap()), 3);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 0, 1, 2]).unwrap()), 4);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 0, 0]).unwrap()), 5);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 0, 1]).unwrap()), 6);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 0, 2]).unwrap()), 7);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 1, 0]).unwrap()), 8);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 1, 1]).unwrap()), 9);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 1, 2]).unwrap()), 10);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 2, 0]).unwrap()), 11);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 2, 1]).unwrap()), 12);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 2, 2]).unwrap()), 13);
        assert_eq!(mesh.get_index(&RestrictedGrowthString::try_from(vec![0, 1, 2, 3]).unwrap()), 14);
    }
}
