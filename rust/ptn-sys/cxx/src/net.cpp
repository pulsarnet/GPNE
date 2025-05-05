// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <ptn/net.h>
#include <ptn/place.h>
#include <ptn/slice.h>

namespace net = ptn::net;
namespace vertex = net::vertex;
namespace edge = net::edge;
namespace vec = ptn::alloc::vec;

extern "C" {
    net::PetriNet* ptn$net$new();
    net::PetriNet* ptn$net$clone(const net::PetriNet*);
    void ptn$net$places(const net::PetriNet*, vec::RustVec<usize>*);
    void ptn$net$transitions(const net::PetriNet*, vec::RustVec<usize>*);
    rust::repr::Fat ptn$net$directed$arcs(const net::PetriNet&);
    rust::repr::Fat ptn$net$inhibitor$arcs(const net::PetriNet&);

    vertex::Place* ptn$net$place(net::PetriNet*, usize);
    usize ptn$net$places_count(const net::PetriNet*);
    usize ptn$net$add_place(net::PetriNet*);
    usize ptn$net$insert_place(net::PetriNet*, usize);
    usize ptn$net$remove_place(net::PetriNet*, usize);

    vertex::Transition* ptn$net$transition(net::PetriNet*, usize);
    usize ptn$net$transitions_count(const net::PetriNet*);
    usize ptn$net$add_transition(net::PetriNet*);
    usize ptn$net$insert_transition(net::PetriNet*, usize);
    void ptn$net$remove_transition(net::PetriNet*, usize);

    edge::DirectedEdge* ptn$net$directed$arc(const net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);
    void ptn$net$add_directed(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex, uint32_t);
    void ptn$net$remove_directed(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);

    edge::InhibitorEdge* ptn$net$inhibitor$arc(const net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);
    bool ptn$net$add_inhibitor(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);
    void ptn$net$remove_inhibitor(net::PetriNet*, vertex::VertexIndex, vertex::VertexIndex);

    void ptn$net$as_matrix(const net::PetriNet*, ptn::matrix::RustMatrix<i32>&, ptn::matrix::RustMatrix<i32>&);

    void ptn$net$clear(net::PetriNet*);
    void ptn$net$drop(net::PetriNet*);
}

net::PetriNet* net::PetriNet::create() { return ptn$net$new(); }

ptn::Box<net::PetriNet> net::PetriNet::create_boxed() { return Box<PetriNet>::from_raw(create()); }

net::PetriNet* net::PetriNet::clone(PetriNet *source) {
    return ptn$net$clone(source);
}

vec::RustVec<usize> net::PetriNet::places() const {
    vec::RustVec<usize> vec;
    ptn$net$places(this, &vec);
    return std::move(vec);
}

vec::RustVec<usize> net::PetriNet::transitions() const {
    vec::RustVec<usize> vec;
    ptn$net$transitions(this, &vec);
    return std::move(vec);
}

rust::Slice<edge::DirectedEdge> net::PetriNet::directed_arcs() const
{
    return rust::impl<rust::Slice<edge::DirectedEdge>>::slice(ptn$net$directed$arcs(*this));
}

rust::Slice<edge::InhibitorEdge> net::PetriNet::inhibitor_arcs() const
{
    return rust::impl<rust::Slice<edge::InhibitorEdge>>::slice(ptn$net$inhibitor$arcs(*this));
}

vertex::Place* net::PetriNet::place(usize id) { return ptn$net$place(this, id); }

usize net::PetriNet::places_count() const
{
    return ptn$net$places_count(this);
}

usize net::PetriNet::add_place() {
    return ptn$net$add_place(this);
}

bool net::PetriNet::insert_place(usize idx) {
    return ptn$net$insert_place(this, idx);
}

usize net::PetriNet::add_transition() {
    return ptn$net$add_transition(this);
}

bool net::PetriNet::insert_transition(usize idx) {
    return ptn$net$insert_transition(this, idx);
}

bool net::PetriNet::remove_place(usize id) {
    ptn$net$remove_place(this, id);
    return true;
}

vertex::Transition* net::PetriNet::transition(usize idx) { return ptn$net$transition(this, idx); }

usize net::PetriNet::transitions_count() const
{
    return ptn$net$transitions_count(this);
}

bool net::PetriNet::remove_transition(usize id) {
    ptn$net$remove_transition(this, id);
    return true;
}

usize net::PetriNet::add_vertex(vertex::VertexType type) {
    if (type == vertex::TPlace)
        return this->add_place();
    return this->add_transition();
}

bool net::PetriNet::remove_vertex(const vertex::VertexIndex index) {
    if (index.t == vertex::TPlace)
        return remove_place(index.id);

    if (index.t == vertex::TTransition)
        return remove_transition(index.id);

    return false;
}

edge::DirectedEdge* net::PetriNet::directed_arc(const vertex::VertexIndex from, const vertex::VertexIndex to) const {
    return ptn$net$directed$arc(this, from, to);
}

void net::PetriNet::add_directed(const vertex::VertexIndex from, const vertex::VertexIndex to, uint32_t weight) {
    ptn$net$add_directed(this, from, to, weight);
}

void net::PetriNet::remove_directed(const vertex::VertexIndex from, const vertex::VertexIndex to) {
    ptn$net$remove_directed(this, from, to);
}

edge::InhibitorEdge* net::PetriNet::inhibitor_arc(const vertex::VertexIndex from, const vertex::VertexIndex to) const {
    return ptn$net$inhibitor$arc(this, from, to);
}

bool net::PetriNet::add_inhibitor(const vertex::VertexIndex from, const vertex::VertexIndex to) {
    return ptn$net$add_inhibitor(this, from, to);
}

void net::PetriNet::remove_inhibitor(const vertex::VertexIndex from, const vertex::VertexIndex to) {
    ptn$net$remove_directed(this, from, to);
}

std::tuple<ptn::matrix::RustMatrix<i32>, ptn::matrix::RustMatrix<i32>> net::PetriNet::as_matrix() const {
    matrix::RustMatrix<i32> input;
    matrix::RustMatrix<i32> output;
    ptn$net$as_matrix(this, input, output);
    return std::make_tuple(std::move(input), std::move(output));
}

void net::PetriNet::clear() {
    ptn$net$clear(this);
}

void net::PetriNet::drop(PetriNet* net) {
    ptn$net$drop(net);
}

template <> net::PetriNet* ptn::allocate() { return ptn$net$new(); }

template <> void ptn::deallocate(net::PetriNet* obj) { return net::PetriNet::drop(obj); }