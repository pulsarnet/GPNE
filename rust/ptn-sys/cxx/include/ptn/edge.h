// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PTN_EDGE_H
#define PTN_EDGE_H

#include <ptn/layout.h>
#include <ptn/place.h>

namespace ptn::net::edge {
    struct DirectedEdge : public Opaque {
        [[nodiscard]] vertex::VertexIndex from() const;
        [[nodiscard]] vertex::VertexIndex to() const;

        void set_weight(uint32_t weight);
        [[nodiscard]] uint32_t weight() const;

        ~DirectedEdge() = delete;

    private:
        friend ::rust::layout;
        struct layout {
            static size_t size() noexcept;
            static size_t align() noexcept;
        };
    };

    struct InhibitorEdge : public Opaque {
        [[nodiscard]] vertex::VertexIndex place() const;
        [[nodiscard]] vertex::VertexIndex transition() const;

        ~InhibitorEdge() = delete;

    private:
        friend ::rust::layout;
        struct layout {
            static size_t size() noexcept;
            static size_t align() noexcept;
        };
    };
}

#endif
