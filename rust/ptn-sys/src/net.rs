/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use crate::matrix::RustMatrix;
use crate::vec::RustVec;
use std::ptr;
use tracing::info;

use crate::slice::RustSlice;
use ptn::net::place::Place;
use ptn::net::transition::Transition;
use ptn::net::vertex::VertexIndex;
use ptn::net::{DirectedEdge, InhibitorEdge, PetriNet};

#[export_name = "ptn$net$new"]
extern "C" fn new_net() -> *mut PetriNet {
    Box::into_raw(Box::new(PetriNet::new()))
}

#[export_name = "ptn$net$clone"]
extern "C" fn clone_net(this: &PetriNet) -> *mut PetriNet {
    Box::into_raw(Box::new(this.clone()))
}

#[export_name = "ptn$net$drop"]
extern "C" fn delete_net(v: *mut PetriNet) {
    let _ = unsafe { Box::from_raw(v) };
}

#[export_name = "ptn$net$place"]
extern "C" fn net_place(net: &mut PetriNet, id: usize) -> *const Place {
    net.place(id).map_or(ptr::null(), |p| p as *const _)
}

#[export_name = "ptn$net$places_count"]
extern "C" fn net_places_count(net: &PetriNet) -> usize {
    net.places().len()
}

#[export_name = "ptn$net$transition"]
extern "C" fn net_transition(net: &mut PetriNet, id: usize) -> *const Transition {
    net.transition(id).map_or(ptr::null(), |t| t as *const _)
}

#[export_name = "ptn$net$transitions_count"]
extern "C" fn net_transitions_count(net: &PetriNet) -> usize {
    net.transitions().len()
}

#[export_name = "ptn$net$places"]
extern "C" fn net_places(net: &mut PetriNet, ret: &mut RustVec<usize>) {
    let result = net.places().keys().copied().collect::<Vec<_>>();

    unsafe { ptr::write_unaligned(ret, RustVec::from(result)) };
}

#[export_name = "ptn$net$transitions"]
extern "C" fn net_transitions(net: &mut PetriNet, ret: &mut RustVec<usize>) {
    let result = net.transitions().keys().copied().collect::<Vec<_>>();

    unsafe { ptr::write_unaligned(ret, RustVec::from(result)) };
}

#[export_name = "ptn$net$directed$arcs"]
unsafe extern "C" fn net_directed_arcs(net: &PetriNet) -> RustSlice {
    RustSlice::from_ref(net.directed())
}

#[export_name = "ptn$net$directed$arc"]
unsafe extern "C" fn net_directed_arc(
    net: &mut PetriNet,
    from: VertexIndex,
    to: VertexIndex,
) -> *const DirectedEdge {
    net.edges()
        .get_directed(from, to)
        .map(|conn| conn as _)
        .unwrap_or(ptr::null())
}

#[export_name = "ptn$net$inhibitor$arcs"]
unsafe extern "C" fn net_inhibitor_arcs(net: &PetriNet) -> RustSlice {
    RustSlice::from_ref(net.edges().inhibitor())
}

#[export_name = "ptn$net$inhibitor$arc"]
unsafe extern "C" fn net_inhibitor_arc(
    net: &mut PetriNet,
    place: VertexIndex,
    transition: VertexIndex,
) -> *const InhibitorEdge {
    net.edges()
        .get_inhibitor(place, transition)
        .map(|conn| conn as _)
        .unwrap_or(ptr::null())
}

#[export_name = "ptn$net$clear"]
extern "C" fn net_clear(v: &mut PetriNet) {
    *v = PetriNet::new();
}

#[export_name = "ptn$net$add_place"]
extern "C" fn add_place(net: &mut PetriNet) -> *const Place {
    net.add_place(Place::default()) as *const Place
}

#[export_name = "ptn$net$insert_place"]
extern "C" fn insert_place(net: &mut PetriNet, index: usize) -> *const Place {
    if !net.insert_place(index, Place::default(), None) {
        return ptr::null();
    }
    net.place(index).unwrap() as *const Place
}

#[export_name = "ptn$net$remove_place"]
extern "C" fn remove_place(net: &mut PetriNet, index: usize) {
    net.remove_place(index)
}

#[export_name = "ptn$net$add_transition"]
extern "C" fn add_transition(net: &mut PetriNet) -> *const Transition {
    net.add_transition(Transition::default()) as *const Transition
}

#[export_name = "ptn$net$insert_transition"]
extern "C" fn insert_transition(net: &mut PetriNet, index: usize) -> *const Transition {
    if !net.insert_transition(index, Transition::default(), None) {
        return ptr::null();
    }
    net.transition(index).unwrap() as *const Transition
}

#[export_name = "ptn$net$remove_transition"]
extern "C" fn remove_transition(net: &mut PetriNet, index: usize) {
    net.remove_transition(index)
}

#[export_name = "ptn$net$add_directed"]
extern "C" fn add_directed(net: &mut PetriNet, from: VertexIndex, to: VertexIndex, weight: u32) {
    info!("add directed edge to net: {net:?}");
    net.add_directed(DirectedEdge::new_with(from, to, weight));
}

/// Deletes all vertex junctions that exit from and enter to
#[export_name = "ptn$net$remove_directed"]
extern "C" fn remove_directed(net: &mut PetriNet, from: VertexIndex, to: VertexIndex) {
    net.remove_directed(from, to);
}

#[export_name = "ptn$net$add_inhibitor"]
extern "C" fn add_inhibitor(
    net: &mut PetriNet,
    place: VertexIndex,
    transition: VertexIndex,
) -> bool {
    info!("add inhibitor edge to net: {net:?}");
    net.add_inhibitor(InhibitorEdge::new(place, transition))
}

/// Deletes all vertex junctions that exit from and enter to
#[export_name = "ptn$net$remove_inhibitor"]
extern "C" fn remove_inhibitor(net: &mut PetriNet, place: VertexIndex, transition: VertexIndex) {
    net.remove_inhibitor(place, transition);
}

#[export_name = "ptn$net$as_matrix"]
extern "C" fn as_matrix(net: &PetriNet, input: &mut RustMatrix<i32>, output: &mut RustMatrix<i32>) {
    let (i, o) = net.adjacency_matrices::<i32>();
    unsafe {
        ptr::write(input, RustMatrix::from(i));
        ptr::write(output, RustMatrix::from(o));
    }
}
