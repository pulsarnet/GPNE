/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

#![allow(missing_docs)]

use core::mem::{self, MaybeUninit};
use core::ptr::{self, NonNull};
use core::slice;

// ABI compatible with C++ rust::Slice<T> (not necessarily &[T]).
#[repr(C)]
pub struct RustSlice {
    repr: [MaybeUninit<usize>; mem::size_of::<NonNull<[()]>>() / mem::size_of::<usize>()],
}

impl RustSlice {
    pub fn from_ref<T>(slice: &[T]) -> Self {
        let ptr = NonNull::from(slice).cast::<T>();
        let len = slice.len();
        Self::from_raw_parts(ptr, len)
    }

    pub fn from_mut<T>(slice: &mut [T]) -> Self {
        let ptr = NonNull::from(&mut *slice).cast::<T>();
        let len = slice.len();
        Self::from_raw_parts(ptr, len)
    }

    pub unsafe fn as_slice<'a, T>(self) -> &'a [T] {
        let ptr = self.as_non_null_ptr().as_ptr();
        let len = self.len();
        unsafe { slice::from_raw_parts(ptr, len) }
    }

    pub unsafe fn as_mut_slice<'a, T>(self) -> &'a mut [T] {
        let ptr = self.as_non_null_ptr().as_ptr();
        let len = self.len();
        unsafe { slice::from_raw_parts_mut(ptr, len) }
    }

    pub(crate) fn from_raw_parts<T>(ptr: NonNull<T>, len: usize) -> Self {
        let ptr = ptr::slice_from_raw_parts_mut(ptr.as_ptr().cast(), len);
        unsafe { mem::transmute::<NonNull<[()]>, RustSlice>(NonNull::new_unchecked(ptr)) }
    }

    pub(crate) fn as_non_null_ptr<T>(&self) -> NonNull<T> {
        let rust_slice = RustSlice { repr: self.repr };
        let repr = unsafe { mem::transmute::<RustSlice, NonNull<[()]>>(rust_slice) };
        repr.cast()
    }

    pub(crate) fn len(&self) -> usize {
        let rust_slice = RustSlice { repr: self.repr };
        let repr = unsafe { mem::transmute::<RustSlice, NonNull<[()]>>(rust_slice) };
        unsafe { repr.as_ref() }.len()
    }
}

#[export_name = "ptn$slice$new"]
unsafe extern "C" fn slice_new(this: &mut MaybeUninit<RustSlice>, ptr: NonNull<()>, len: usize) {
    let this = this.as_mut_ptr();
    let rust_slice = RustSlice::from_raw_parts(ptr, len);
    unsafe { ptr::write(this, rust_slice) }
}

#[export_name = "ptn$slice$ptr"]
unsafe extern "C" fn slice_ptr(this: &RustSlice) -> NonNull<()> {
    this.as_non_null_ptr()
}

#[export_name = "ptn$slice$len"]
unsafe extern "C" fn slice_len(this: &RustSlice) -> usize {
    this.len()
}
