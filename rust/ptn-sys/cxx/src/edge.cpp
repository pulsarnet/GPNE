// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <ptn/edge.h>
#include <ptn/place.h>

namespace vertex = ptn::net::vertex;
namespace edge = ptn::net::edge;

extern "C" {
    vertex::VertexIndex ptn$net$edge$directed$from(const edge::DirectedEdge* edge);
    vertex::VertexIndex ptn$net$edge$directed$to(const edge::DirectedEdge* edge);
    uint32_t ptn$net$edge$directed$weight(const edge::DirectedEdge* edge);
    void ptn$net$edge$directed$set_weight(edge::DirectedEdge* edge, uint32_t weight);
    size_t ptn$net$edge$directed$sizeof();
    size_t ptn$net$edge$directed$alignof();

    vertex::VertexIndex ptn$net$edge$inhibitor$place(const edge::InhibitorEdge* edge);
    vertex::VertexIndex ptn$net$edge$inhibitor$transition(const edge::InhibitorEdge* edge);
    size_t ptn$net$edge$inhibitor$sizeof();
    size_t ptn$net$edge$inhibitor$alignof();
}

vertex::VertexIndex edge::DirectedEdge::from() const {
    return ptn$net$edge$directed$from(this);
}

vertex::VertexIndex edge::DirectedEdge::to() const {
    return ptn$net$edge$directed$to(this);
}

uint32_t edge::DirectedEdge::weight() const { return ptn$net$edge$directed$weight(this); }

size_t edge::DirectedEdge::layout::size() noexcept { return ptn$net$edge$directed$sizeof(); }

size_t edge::DirectedEdge::layout::align() noexcept { return ptn$net$edge$directed$alignof(); }

void edge::DirectedEdge::set_weight(const uint32_t weight) {
    return ptn$net$edge$directed$set_weight(this, weight);
}

vertex::VertexIndex edge::InhibitorEdge::place() const {
    return ptn$net$edge$inhibitor$place(this);
}

vertex::VertexIndex edge::InhibitorEdge::transition() const
{
    return ptn$net$edge$inhibitor$transition(this);
}

size_t edge::InhibitorEdge::layout::size() noexcept { return ptn$net$edge$inhibitor$sizeof(); }

size_t edge::InhibitorEdge::layout::align() noexcept { return ptn$net$edge$inhibitor$alignof(); }