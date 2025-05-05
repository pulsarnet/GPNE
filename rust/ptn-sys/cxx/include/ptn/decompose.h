// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef DECOMPOSE_H
#define DECOMPOSE_H

#include <ptn/types.h>
#include <ptn/opaque.h>

#include "matrix.h"
#include "slice.h"
#include "vec.h"

namespace ptn::net {
    struct PetriNet;
}

namespace ptn::modules::decompose {
    struct DecomposeContext : public Opaque {
        static DecomposeContext* init(const net::PetriNet*);
        [[nodiscard]] usize positions() const;
        [[nodiscard]] usize transitions() const;
        [[nodiscard]] net::PetriNet* primitive() const;
        [[nodiscard]] net::PetriNet* lbf() const;
        [[nodiscard]] usize position_at() const;
        [[nodiscard]] usize transition_at() const;
        static void drop(DecomposeContext*);

        // synthesis
        matrix::RustMatrix<f64> tensor() const noexcept;
        [[nodiscard]] net::PetriNet* eval_program(rust::Slice<u16> placesRGS, rust::Slice<u16> transitionRGS) const;

        ~DecomposeContext() = delete;
    };
}

#endif //DECOMPOSE_H
