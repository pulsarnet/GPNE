/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use ptn::net::vertex::VertexIndex;
use ptn::net::DirectedEdge;
use ptn::net::InhibitorEdge;

#[export_name = "ptn$net$edge$directed$from"]
extern "C" fn edge_from(edge: &DirectedEdge) -> VertexIndex {
    edge.begin()
}

#[export_name = "ptn$net$edge$directed$to"]
extern "C" fn edge_to(edge: &DirectedEdge) -> VertexIndex {
    edge.end()
}

#[export_name = "ptn$net$edge$directed$weight"]
extern "C" fn edge_weight(edge: &DirectedEdge) -> u32 {
    edge.weight()
}

#[export_name = "ptn$net$edge$directed$set_weight"]
pub extern "C" fn edge_set_weight(edge: &mut DirectedEdge, weight: u32) {
    edge.set_weight(weight);
}

#[export_name = "ptn$net$edge$directed$sizeof"]
pub extern "C" fn edge_directed_sizeof() -> usize {
    std::alloc::Layout::new::<DirectedEdge>().size()
}

#[export_name = "ptn$net$edge$directed$alignof"]
pub extern "C" fn edge_directed_alignof() -> usize {
    std::alloc::Layout::new::<DirectedEdge>().align()
}

#[export_name = "ptn$net$edge$inhibitor$place"]
extern "C" fn inhibitor_place(edge: &InhibitorEdge) -> VertexIndex {
    edge.place()
}

#[export_name = "ptn$net$edge$inhibitor$transition"]
extern "C" fn inhibitor_transition(edge: &InhibitorEdge) -> VertexIndex {
    edge.transition()
}

#[export_name = "ptn$net$edge$inhibitor$sizeof"]
pub extern "C" fn edge_inhibitor_sizeof() -> usize {
    std::alloc::Layout::new::<InhibitorEdge>().size()
}

#[export_name = "ptn$net$edge$inhibitor$alignof"]
pub extern "C" fn edge_inhibitor_alignof() -> usize {
    std::alloc::Layout::new::<InhibitorEdge>().align()
}

