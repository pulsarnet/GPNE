/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

extern crate cxx;
extern crate libc;
extern crate num_traits;
extern crate ptn;
extern crate tracing;
extern crate tracing_subscriber;

mod decompose;
mod edge;
mod invariant;
mod logger;
mod matrix;
mod net;
mod place;
mod reachability;
mod simulation;
mod vec;
mod slice;
