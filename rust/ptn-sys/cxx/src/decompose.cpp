// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <ptn/decompose.h>
#include <ptn/net.h>

namespace ptn::modules::decompose {
    extern "C" {
        DecomposeContext* ptn$modules$decompose$init(const net::PetriNet* net);
        usize ptn$modules$decompose$positions(const DecomposeContext* ctx);
        usize ptn$modules$decompose$transitions(const DecomposeContext* ctx);
        net::PetriNet* ptn$modules$decompose$primitive(const DecomposeContext* ctx);
        net::PetriNet* ptn$modules$decompose$lbf(const DecomposeContext* ctx);
        usize ptn$modules$decompose$position_at(const DecomposeContext* ctx);
        usize ptn$modules$decompose$transition_at(const DecomposeContext* ctx);
        void ptn$modules$decompose$tensor(const DecomposeContext* ctx, matrix::RustMatrix<f64>*);
        net::PetriNet* ptn$modules$decompose$eval_program(const DecomposeContext* ctx, rust::Slice<u16>, rust::Slice<u16>);
        void ptn$modules$decompose$drop(DecomposeContext* ctx);
    }

    DecomposeContext* DecomposeContext::init(const net::PetriNet* net) {
        return ptn$modules$decompose$init(net);
    }

    usize DecomposeContext::positions() const {
        return ptn$modules$decompose$positions(this);
    }

    usize DecomposeContext::transitions() const {
        return ptn$modules$decompose$transitions(this);
    }

    net::PetriNet* DecomposeContext::primitive() const {
        return ptn$modules$decompose$primitive(this);
    }

    net::PetriNet* DecomposeContext::lbf() const {
        return ptn$modules$decompose$lbf(this);
    }

    usize DecomposeContext::position_at() const {
        return ptn$modules$decompose$position_at(this);
    }

    usize DecomposeContext::transition_at() const {
        return ptn$modules$decompose$transition_at(this);
    }

    matrix::RustMatrix<f64> DecomposeContext::tensor() const noexcept {
        matrix::RustMatrix<f64> matrix;
        ptn$modules$decompose$tensor(this, &matrix);
        return std::move(matrix);
    }

    net::PetriNet* DecomposeContext::eval_program(rust::Slice<u16> placesRGS, rust::Slice<u16> transitionRGS) const {
        return ptn$modules$decompose$eval_program(this, placesRGS, transitionRGS);
    }

    void DecomposeContext::drop(DecomposeContext* self) {
        ptn$modules$decompose$drop(self);
    }
}