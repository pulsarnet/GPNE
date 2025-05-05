// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <ptn/place.h>

extern "C" {
    usize ptn$net$place$markers(const ptn::net::vertex::Place*);
    void ptn$net$place$set_markers(ptn::net::vertex::Place*, usize);
}

usize ptn::net::vertex::Place::markers() const {
    return ptn$net$place$markers(this);
}

void ptn::net::vertex::Place::set_markers(usize markers) {
    ptn$net$place$set_markers(this, markers);
}

