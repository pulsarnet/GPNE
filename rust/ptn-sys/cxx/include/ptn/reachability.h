// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PTN_REACHABILITY_H
#define PTN_REACHABILITY_H

#include "net.h"
#include "opaque.h"
#include "place.h"
#include "slice.h"
#include "types.h"
#include "vec.h"

namespace ptn::modules::reachability {
    struct Marking;
    
    enum CovType {
        DeadEnd = 0,
        Inner = 1,
        Boundary = 2,
        Duplicate = 3,
    };

    struct MarkingIndex {
        int prev;
        net::vertex::VertexIndex transition;
    };

    struct Reachability final : public Opaque {
        static Reachability* init(net::PetriNet*, size_t hardLimit);
        size_t compute(size_t);
        bool hasBoundary() const;
        size_t count() const;
        [[nodiscard]] alloc::vec::RustVec<net::vertex::VertexIndex> positions() const;
        [[nodiscard]] rust::Slice<Marking> marking() const;
        void drop();

        ~Reachability() = delete;
    };

    struct Marking final : public Opaque {
        [[nodiscard]] alloc::vec::RustVec<i32> values() const;
        [[nodiscard]] CovType type() const;
        [[nodiscard]] alloc::vec::RustVec<MarkingIndex> prev() const;
        [[nodiscard]]
        alloc::vec::RustVec<MarkingIndex> next() const;

        ~Marking() = delete;

    private:
        friend ::rust::layout;

        struct layout {
            static size_t size() noexcept;
            static size_t align() noexcept;
        };
    };
}

#endif