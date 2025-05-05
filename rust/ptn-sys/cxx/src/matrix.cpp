// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <ptn/matrix.h>
#include <ptn/types.h>
#include <cassert>
#include <cstddef>

namespace matrix = ptn::matrix;

#define IMPL_FOR_TYPE_INNER(type, name) \
    extern "C" { \
        void ptn$matrix$##name##$new(matrix::RustMatrix<type>*, size_t, size_t); \
        void ptn$matrix$##name##$drop(matrix::RustMatrix<type>*); \
        size_t ptn$matrix$##name##$nrows(const matrix::RustMatrix<type>*); \
        size_t ptn$matrix$##name##$ncols(const matrix::RustMatrix<type>*); \
        void ptn$matrix$##name##$add_row(matrix::RustMatrix<type>*); \
        void ptn$matrix$##name##$remove_row(matrix::RustMatrix<type>*, size_t); \
        void ptn$matrix$##name##$add_column(matrix::RustMatrix<type>*); \
        void ptn$matrix$##name##$remove_column(matrix::RustMatrix<type>*, size_t); \
        void ptn$matrix$##name##$clear(matrix::RustMatrix<type>*); \
        matrix::RustMatrix<type>::const_pointer ptn$matrix$##name##$index(const matrix::RustMatrix<type>*, size_t, size_t); \
    } \
    \
    template<> \
    matrix::RustMatrix<type>::RustMatrix() noexcept { \
        ptn$matrix$##name##$new(this, 0, 0); \
    } \
    \
    template<> \
    matrix::RustMatrix<type>::RustMatrix(size_t n, size_t m) { \
        ptn$matrix$##name##$new(this, n, m); \
    } \
    \
    template<> \
    void matrix::RustMatrix<type>::drop() { \
        ptn$matrix$##name##$drop(this); \
    } \
    \
    template<> \
    void matrix::RustMatrix<type>::add_row() { \
        return ptn$matrix$##name##$add_row(this); \
    } \
    \
    template<> \
    void matrix::RustMatrix<type>::remove_row(size_t i) { \
        return ptn$matrix$##name##$remove_row(this, i); \
    } \
    \
    template<> \
    void matrix::RustMatrix<type>::add_column() { \
        return ptn$matrix$##name##$add_column(this); \
    } \
    \
    template<> \
    void matrix::RustMatrix<type>::remove_column(size_t i) { \
        return ptn$matrix$##name##$remove_column(this, i); \
    } \
    \
    template<> \
    void matrix::RustMatrix<type>::clear() { \
        return ptn$matrix$##name##$clear(this); \
    } \
    template<> \
    size_t matrix::RustMatrix<type>::nrows() const noexcept { \
        return ptn$matrix$##name##$nrows(this); \
    } \
    \
    template<> \
    size_t matrix::RustMatrix<type>::ncols() const noexcept { \
        return ptn$matrix$##name##$ncols(this); \
    } \
    \
    template <> \
    typename matrix::RustMatrix<type>::const_reference matrix::RustMatrix<type>::operator[](index i) const { \
        assert(i.row < this->nrows()); \
        assert(i.col < this->ncols()); \
        return *ptn$matrix$##name##$index(this, i.row, i.col); \
    } \

#define IMPL_FOR_TYPE(type) IMPL_FOR_TYPE_INNER(type, type)

IMPL_FOR_TYPE(i32)
IMPL_FOR_TYPE(i64)
IMPL_FOR_TYPE(f64)