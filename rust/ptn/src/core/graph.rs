/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

pub mod johnson_circuit;
mod path;
mod strongly_connected;

pub use self::path::NetPaths;
use crate::net::vertex::{VertexIndex, VertexType};
use crate::net::PetriNet;
use std::collections::HashSet;

/// Get adjacent vertices to the given vertex
fn adjacent(net: &PetriNet, idx: VertexIndex) -> Vec<VertexIndex> {
    let adjacent = net
        .edges()
        .directed()
        .iter()
        .filter(|edge| edge.begin() == idx)
        .map(|edge| edge.end())
        .fold(HashSet::new(), |mut set, idx| {
            set.insert(idx);
            set
        });

    let adjacent = net
        .edges()
        .inhibitor()
        .iter()
        .filter(|edge| match idx.type_ {
            VertexType::Place => edge.place() == idx,
            VertexType::Transition => edge.transition() == idx,
        })
        .map(|edge| match idx.type_ {
            VertexType::Place => edge.transition(),
            VertexType::Transition => edge.place(),
        })
        .fold(adjacent, |mut set, idx| {
            set.insert(idx);
            set
        });

    adjacent.into_iter().collect()
}

#[cfg(test)]
mod tests {
    use crate::net::{DirectedEdge, PetriNet};
    use crate::net::place::Place;
    use crate::net::transition::Transition;
    use crate::net::vertex::VertexIndex;

    #[test]
    fn test_adjacent() {
        let mut net = PetriNet::new();
        let p1 = VertexIndex::place(net.add_place(Place::default()));
        let p2 = VertexIndex::place(net.add_place(Place::default()));
        let t1 = VertexIndex::transition(net.add_transition(Transition::default()));
        let t2 = VertexIndex::transition(net.add_transition(Transition::default()));

        net.add_directed(DirectedEdge::new(p1, t1));
        net.add_directed(DirectedEdge::new(t1, p2));
        net.add_directed(DirectedEdge::new(t1, p1));
        net.add_directed(DirectedEdge::new(p2, t2));
        net.add_directed(DirectedEdge::new(t2, p1));

        let adjacent = super::adjacent(&net, p1);
        assert_eq!(adjacent.len(), 1);
        assert!(adjacent.contains(&t1));

        let adjacent = super::adjacent(&net, t1);
        assert_eq!(adjacent.len(), 2);
        assert!(adjacent.contains(&p2));
        assert!(adjacent.contains(&p1));
    }
}
