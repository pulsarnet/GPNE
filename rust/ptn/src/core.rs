/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

mod graph;
mod mesh;
mod utils;
mod rgs;

pub use self::graph::*;
pub use self::mesh::*;
pub use self::rgs::*;
pub use self::utils::*;
use crate::net::vertex::VertexIndex;
use nalgebra::DMatrix;
use std::cmp::max;
use std::collections::{HashMap, HashSet};
use std::fmt::{Display, Formatter};

pub struct MatrixFormat<'a, T>(
    pub &'a DMatrix<T>,
    pub &'a Vec<VertexIndex>,
    pub &'a Vec<VertexIndex>,
);

impl<'a, T: Display> Display for MatrixFormat<'a, T> {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        let (rows, cols) = self.0.shape();

        if rows == 0 && cols == 0 {
            return write!(f, "[]");
        }

        let mut name_len = 1;
        for name in self.1.iter() {
            let name_len_in = format!("{}", name).chars().count();
            if name_len_in > name_len {
                name_len = name_len_in
            }
        }

        let mut member_len = 1;

        for member in self.0.data.as_slice().iter() {
            let chars = format!("{}", member).chars().count();
            if chars > member_len {
                member_len = chars;
            }
        }

        let max_len = max(name_len, member_len) + 2;

        writeln!(f)?;
        write!(f, " {:>freepad$} ┌", "", freepad = max_len)?;
        for name in self.1.iter() {
            write!(f, "{:^pad$}", name, pad = max_len)?;
        }
        writeln!(f, " ┐")?;

        for row in 0..rows {
            write!(f, " {:^pad$} |", self.2[row], pad = max_len)?;
            for col in 0..cols {
                let member = format!("{}", self.0[(row, col)]);
                if row == col {
                    write!(f, "{:^thepad$}", member, thepad = max_len)?;
                } else {
                    write!(f, "{:^thepad$}", member, thepad = max_len)?;
                }
            }
            writeln!(f, " │")?;
        }

        let width = self.2.len() * max_len - 1;
        writeln!(
            f,
            " {:>freepad$} └ {:>width$} ┘",
            "",
            "",
            freepad = max_len,
            width = width
        )?;

        Ok(())
    }
}

pub struct HashMapFormat<'a>(pub &'a HashMap<VertexIndex, HashSet<VertexIndex>>);

impl<'a> Display for HashMapFormat<'a> {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        writeln!(f, "[")?;

        let max_k_size = self
            .0
            .iter()
            .map(|(k, _)| format!("{}", k).len())
            .max()
            .unwrap_or(0);

        for (i, (k, v)) in self.0.iter().enumerate() {
            write!(f, "\t{:<pad$} -> {{", k, pad = max_k_size)?;

            for (j, el) in v.iter().enumerate() {
                write!(f, "{}", el)?;
                if j != (v.len() - 1) {
                    write!(f, ", ")?;
                }
            }

            write!(f, "}}")?;
            if i != (self.0.len() - 1) {
                write!(f, ",")?;
            }

            writeln!(f, "")?;
        }

        writeln!(f, "]")
    }
}
