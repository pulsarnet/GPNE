/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use std::fmt::{Debug, Display, Formatter};
use std::hash::Hash;

#[repr(C)]
#[derive(PartialEq, Hash, Eq, Clone, Copy, Debug)]
pub enum VertexType {
    Place,
    Transition,
}

impl Default for VertexType {
    fn default() -> Self {
        VertexType::Place
    }
}

#[repr(C)]
#[derive(Default, PartialEq, Hash, Eq, Clone, Copy, Debug)]
pub struct VertexIndex {
    pub type_: VertexType,
    pub id: usize,
}

impl VertexIndex {
    pub fn place(index: usize) -> VertexIndex {
        VertexIndex {
            type_: VertexType::Place,
            id: index,
        }
    }

    pub fn transition(index: usize) -> VertexIndex {
        VertexIndex {
            type_: VertexType::Transition,
            id: index,
        }
    }
}

impl Display for VertexIndex {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self.type_ {
            VertexType::Place => write!(f, "p{}", self.id),
            VertexType::Transition => write!(f, "t{}", self.id),
        }
    }
}
