/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use crate::matrix::RustMatrix;
use ptn::extension::synthesis::{synthesis_program, DecomposeContext, SynthesisProgram};
use ptn::net::PetriNet;
use std::ptr;
use tracing::info;
use ptn::core::RestrictedGrowthString;
use crate::slice::RustSlice;
use crate::vec::RustVec;

#[export_name = "ptn$modules$decompose$init"]
pub extern "C" fn decompose_context_init(net: &PetriNet) -> *mut DecomposeContext {
    Box::into_raw(Box::new(DecomposeContext::init(net)))
}

#[export_name = "ptn$modules$decompose$positions"]
pub extern "C" fn decompose_context_positions(ctx: &DecomposeContext) -> usize {
    ctx.linear_base_fragments_net.places().len()
}

#[export_name = "ptn$modules$decompose$transitions"]
pub extern "C" fn decompose_context_transitions(ctx: &DecomposeContext) -> usize {
    ctx.linear_base_fragments_net.transitions().len()
}

#[export_name = "ptn$modules$decompose$primitive"]
extern "C" fn decompose_context_primitive_net(ctx: &DecomposeContext) -> *const PetriNet {
    &ctx.primitive_net as *const PetriNet
}

#[export_name = "ptn$modules$decompose$lbf"]
extern "C" fn decompose_context_linear_base_fragments(ctx: &DecomposeContext) -> *const PetriNet {
    &ctx.linear_base_fragments_net as *const PetriNet
}

#[export_name = "ptn$modules$decompose$position_at"]
extern "C" fn decompose_context_position_index(ctx: &DecomposeContext, index: usize) -> usize {
    *ctx.linear_base_fragments_net
        .places()
        .get_index(index)
        .expect("out of bound")
        .0
}

#[export_name = "ptn$modules$decompose$transition_at"]
extern "C" fn decompose_context_transition_index(ctx: &DecomposeContext, index: usize) -> usize {
    *ctx.linear_base_fragments_net
        .transitions()
        .get_index(index)
        .expect("out of bound")
        .0
}

#[export_name = "ptn$modules$decompose$tensor"]
extern "C" fn decompose_context_tensor(ctx: &DecomposeContext, matrix: &mut RustMatrix<f64>) {
    unsafe {
        ptr::write(matrix, RustMatrix::from(ctx.c_matrix.clone_owned()));
    }
}

// #[export_name = "ptn$modules$decompose$programs"]
// extern "C" fn decompose_context_programs(ctx: &DecomposeContext) -> usize {
//     ctx.place_set_partitions.max() * ctx.transition_set_partitions.max()
// }

#[export_name = "ptn$modules$decompose$eval_program"]
unsafe extern "C" fn decompose_context_eval_program(
    ctx: &DecomposeContext,
    places: RustSlice,
    transitions: RustSlice,
) -> *const PetriNet {
    let program = SynthesisProgram::new_with(places.as_slice::<u16>(), transitions.as_slice::<u16>());
    Box::into_raw(Box::new(synthesis_program(ctx, program))) as *const PetriNet
}

#[export_name = "ptn$modules$decompose$drop"]
pub unsafe extern "C" fn decompose_context_delete(ctx: *mut DecomposeContext) {
    let _ = Box::from_raw(ctx);
}
