/*
 * SPDX-FileCopyrightText: 2025 Nickolay Muravev
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2025 Nickolay Muravev
 */

use num_traits::Zero;
use ptn::nalgebra::{DMatrix, Scalar};
use std::ffi::c_void;
use std::fmt::Debug;
use std::marker::PhantomData;
use std::mem::MaybeUninit;
use std::{mem, ptr};

#[repr(C)]
pub(crate) struct RustMatrix<T> {
    repr: [MaybeUninit<usize>; mem::size_of::<DMatrix<c_void>>() / mem::size_of::<usize>()],
    marker: PhantomData<DMatrix<T>>,
}

impl<T> RustMatrix<T>
where
    T: Scalar + Debug + Zero,
{
    pub fn new(n: usize, m: usize) -> Self {
        Self::from(DMatrix::<T>::zeros(n, m))
    }

    #[allow(unused)]
    pub fn into_matrix(self) -> DMatrix<T> {
        unsafe { mem::transmute::<RustMatrix<T>, DMatrix<T>>(self) }
    }

    pub fn as_matrix(&self) -> &DMatrix<T> {
        unsafe { &*(self as *const RustMatrix<T> as *const DMatrix<T>) }
    }

    #[allow(unused)]
    pub fn as_mut_matrix(&mut self) -> &mut DMatrix<T> {
        unsafe { &mut *(self as *mut RustMatrix<T> as *mut DMatrix<T>) }
    }

    pub fn add_row(&mut self) {
        *self.as_mut_matrix() = self
            .as_matrix()
            .to_owned()
            .insert_row(self.nrows(), T::zero());
    }

    pub fn remove_row(&mut self, i: usize) {
        *self.as_mut_matrix() = self.as_matrix().to_owned().remove_row(i);
    }

    pub fn add_column(&mut self) {
        *self.as_mut_matrix() = self
            .as_matrix()
            .to_owned()
            .insert_column(self.ncols(), T::zero());
    }

    pub fn remove_column(&mut self, i: usize) {
        *self.as_mut_matrix() = self.as_matrix().to_owned().remove_column(i);
    }

    pub fn clear(&mut self) {
        *self = Self::new(0, 0);
    }

    pub fn nrows(&self) -> usize {
        self.as_matrix().nrows()
    }

    pub fn ncols(&self) -> usize {
        self.as_matrix().ncols()
    }

    pub fn index(&self, row: usize, col: usize) -> *const T {
        unsafe { self.as_matrix().get_unchecked((row, col)) as *const T }
    }
}

impl<T> From<DMatrix<T>> for RustMatrix<T> {
    fn from(v: DMatrix<T>) -> Self {
        unsafe { mem::transmute::<DMatrix<T>, RustMatrix<T>>(v) }
    }
}

macro_rules! rust_matrix_shims {
    ($segment:expr, $ty:ty) => {
        const _: () = {
            #[export_name = concat!("ptn$matrix$", $segment, "$new")]
            unsafe extern "C" fn __new(this: *mut RustMatrix<$ty>, n: usize, m: usize) {
                ptr::write(this, RustMatrix::new(n, m))
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$clear")]
            unsafe extern "C" fn __clear(this: *mut RustMatrix<$ty>) {
                (&mut *this).clear();
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$drop")]
            unsafe extern "C" fn __drop(this: *mut RustMatrix<$ty>) {
                ptr::drop_in_place(this)
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$add_row")]
            unsafe extern "C" fn __add_row(this: *mut RustMatrix<$ty>) {
                (&mut *this).add_row();
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$remove_row")]
            unsafe extern "C" fn __remove_row(this: *mut RustMatrix<$ty>, i: usize) {
                (&mut *this).remove_row(i);
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$add_column")]
            unsafe extern "C" fn __add_column(this: *mut RustMatrix<$ty>) {
                (&mut *this).add_column();
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$remove_column")]
            unsafe extern "C" fn __remove_column(this: *mut RustMatrix<$ty>, i: usize) {
                (&mut *this).remove_column(i);
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$nrows")]
            unsafe extern "C" fn __nrows(this: *const RustMatrix<$ty>) -> usize {
                (&*this).nrows()
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$ncols")]
            unsafe extern "C" fn __ncols(this: *const RustMatrix<$ty>) -> usize {
                (&*this).ncols()
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$index")]
            unsafe extern "C" fn __index(
                this: *const RustMatrix<$ty>,
                row: usize,
                col: usize,
            ) -> *const $ty {
                (&*this).index(row, col)
            }
        };
    };
}

macro_rules! rust_matrix {
    ($t:ident) => {
        rust_matrix_shims! { stringify!($t), $t }
    };
}

rust_matrix!(i32);
rust_matrix!(i64);
rust_matrix!(f64);
