// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PTN_OPAQUE_H
#define PTN_OPAQUE_H

namespace ptn {
    class Opaque {
    public:
        Opaque() = delete;
        Opaque(const Opaque &) = delete;
        ~Opaque() = delete;
    };
}

#endif