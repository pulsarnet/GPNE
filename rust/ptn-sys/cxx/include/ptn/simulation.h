// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SIMULATION_H
#define SIMULATION_H

#include <ptn/opaque.h>
#include <ptn/types.h>
#include <ptn/place.h>
#include <ptn/vec.h>

namespace ptn::net {
    struct PetriNet;
}

namespace vertex = ptn::net::vertex;
namespace vec = ptn::alloc::vec;

namespace ptn::modules::simulation {

    struct Simulation : public Opaque {
        static Simulation* init(const net::PetriNet*);
        i32 step();
        [[nodiscard]] usize markers_at(vertex::VertexIndex) const;
        [[nodiscard]] usize cycles() const;
        [[nodiscard]] vec::RustVec<vertex::VertexIndex> fired() const;
        [[nodiscard]] bool is_fired(vertex::VertexIndex) const;
        //[[nodiscard]] net::PetriNet* net() const;
        void drop();

        ~Simulation() = delete;
    };

}

#endif //SIMULATION_H
