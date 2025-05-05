// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PTN_NET_VERTEX
#define PTN_NET_VERTEX

#include <tuple>
#include <ptn/types.h>

namespace ptn::net::vertex {

    enum VertexType {
        TPlace = 0,
        TTransition = 1
    };

    struct VertexIndex {
        VertexType t;
        usize id;

        bool operator ==(const VertexIndex& rsh) const noexcept { return this->t == rsh.t && this->id == rsh.id; }
        bool operator !=(const VertexIndex& rsh) const noexcept { return !(*this == rsh); }
        friend bool operator <(const VertexIndex& lsh, const VertexIndex& rsh) noexcept { return std::tie(lsh.t, lsh.id) < std::tie(rsh.t, rsh.id); }
    };

    struct Place {
        [[nodiscard]] usize markers() const;
        void add_marker();
        void remove_marker();
        void set_markers(usize);
    };

    struct Transition {

    };

}

#endif