/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use crate::rust_vec_shims;
use crate::slice::RustSlice;
use crate::vec::RustVec;
use ptn::extension::reachability::{CovType, MarkerValue, Marking, Reachability};
use ptn::net::vertex::VertexIndex;
use ptn::net::PetriNet;

#[repr(C)]
struct MarkingIndex {
    idx: i32,
    transition: VertexIndex,
}

#[export_name = "ptn$modules$reachability$init"]
extern "C" fn init(net: &PetriNet, hard_limit: usize) -> *const Reachability {
    Box::into_raw(Box::new(Reachability::new(net, hard_limit)))
}

#[export_name = "ptn$modules$reachability$compute"]
extern "C" fn compute(this: &mut Reachability, limit: usize) -> usize {
    this.compute(limit)
}

#[export_name = "ptn$modules$reachability$hasboundary"]
extern "C" fn has_boundary(this: &mut Reachability) -> bool {
    this.has_boundary()
}

#[export_name = "ptn$modules$reachability$count"]
extern "C" fn count(this: &Reachability) -> usize {
    this.graph().count()
}

#[export_name = "ptn$modules$reachability$positions"]
extern "C" fn positions(this: &Reachability, vec: &mut RustVec<VertexIndex>) {
    unsafe { core::ptr::write_unaligned(vec, RustVec::from(this.graph().positions().to_vec())) };
}

#[export_name = "ptn$modules$reachability$marking"]
extern "C" fn marking(this: &Reachability) -> RustSlice {
    RustSlice::from_ref(this.graph().markings())
}

#[export_name = "ptn$modules$reachability$drop"]
extern "C" fn drop(this: *mut Reachability) {
    if !this.is_null() {
        let _ = unsafe { Box::from_raw(this) };
    }
}

#[export_name = "ptn$modules$reachability$marking$values"]
extern "C" fn marking_values(this: &Marking, vec: &mut RustVec<i32>) {
    let result = this
        .data()
        .row(0)
        .iter()
        .map(MarkerValue::as_number)
        .collect::<Vec<_>>();
    unsafe { core::ptr::write_unaligned(vec, RustVec::from(result)) };
}

#[export_name = "ptn$modules$reachability$marking$type"]
extern "C" fn marking_type(this: &Marking) -> CovType {
    this.r#type()
}

#[export_name = "ptn$modules$reachability$marking$prev"]
extern "C" fn marking_prev(this: &Marking, vec: &mut RustVec<MarkingIndex>) {
    let result = this
        .prev()
        .iter()
        .map(|(transition, idx)| MarkingIndex { transition: *transition, idx: *idx as i32 })
        .collect::<Vec<_>>();
    unsafe { core::ptr::write_unaligned(vec, RustVec::from(result)) }
}

#[export_name = "ptn$modules$reachability$marking$next"]
extern "C" fn marking_next(this: &Marking, vec: &mut RustVec<MarkingIndex>) {
    let result = this
        .next()
        .iter()
        .map(|(transition, idx)| MarkingIndex { transition: *transition, idx: *idx as i32 })
        .collect::<Vec<_>>();
    unsafe { core::ptr::write_unaligned(vec, RustVec::from(result)) }
}

#[export_name = "ptn$modules$reachability$marking$sizeof"]
extern "C" fn size_of() -> usize {
    std::alloc::Layout::new::<Marking>().size()
}

#[export_name = "ptn$modules$reachability$marking$alignof"]
extern "C" fn align_of() -> usize {
    std::alloc::Layout::new::<Marking>().align()
}

rust_vec_shims!("marking_index", MarkingIndex);

