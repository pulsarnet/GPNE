// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PTN_NET
#define PTN_NET
#include "box.h"

#include <ptn/edge.h>
#include <ptn/opaque.h>
#include <ptn/place.h>
#include <ptn/slice.h>
#include <ptn/vec.h>

#include "matrix.h"

namespace ptn::net {

    struct PetriNet final: public Opaque {
        static PetriNet* create();
        static Box<PetriNet> create_boxed();
        static PetriNet* clone(PetriNet* source);
        [[nodiscard]] alloc::vec::RustVec<usize> places() const;
        [[nodiscard]] alloc::vec::RustVec<usize> transitions() const;
        [[nodiscard]]
        rust::Slice<edge::DirectedEdge> directed_arcs() const;
        [[nodiscard]]
        rust::Slice<edge::InhibitorEdge> inhibitor_arcs() const;

        vertex::Place* place(usize);
        [[nodiscard]] usize places_count() const;
        usize add_place();
        bool insert_place(usize);
        bool remove_place(usize);

        vertex::Transition* transition(usize);
        [[nodiscard]] usize transitions_count() const;
        usize add_transition();
        bool insert_transition(usize);
        bool remove_transition(usize);

        usize add_vertex(vertex::VertexType);
        bool insert_vertex(vertex::VertexIndex);
        bool remove_vertex(vertex::VertexIndex);

        // edges
        [[nodiscard]] edge::DirectedEdge* directed_arc(vertex::VertexIndex, vertex::VertexIndex) const;
        void add_directed(vertex::VertexIndex, vertex::VertexIndex, uint32_t weight = 1);
        void remove_directed(vertex::VertexIndex, vertex::VertexIndex);

        [[nodiscard]] edge::InhibitorEdge* inhibitor_arc(vertex::VertexIndex, vertex::VertexIndex) const;
        bool add_inhibitor(vertex::VertexIndex, vertex::VertexIndex);
        void remove_inhibitor(vertex::VertexIndex, vertex::VertexIndex);

        // representation
        [[nodiscard]] std::tuple<matrix::RustMatrix<i32>, matrix::RustMatrix<i32>> as_matrix() const;

        void clear();
        static void drop(PetriNet*);

        ~PetriNet() = delete;
    };

}

#endif