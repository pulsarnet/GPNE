// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PTN_TYPES_H
#define PTN_TYPES_H
#include <cstdint>
#if defined(__APPLE__) || defined(__linux__)
#include <cstddef>
#endif

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef double f64;
typedef size_t usize;
typedef ptrdiff_t isize;

#endif //TYPES_H
