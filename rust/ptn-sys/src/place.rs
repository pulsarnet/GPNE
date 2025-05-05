/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use ptn::net::place::Place;

#[export_name = "ptn$net$place$markers"]
extern "C" fn markers(p: *const Place) -> usize {
    let p = unsafe { &*p };
    p.markers()
}

#[export_name = "ptn$net$place$set_markers"]
extern "C" fn set_markers(p: *mut Place, markers: usize) {
    let p = unsafe { &mut *p };
    p.set_markers(markers);
}
