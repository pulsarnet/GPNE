// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <ptn/slice.h>

extern "C" {
    // rust::Slice
    void ptn$slice$new(void *self, const void *ptr, std::size_t len) noexcept;
    void* ptn$slice$ptr(const void *self) noexcept;
    std::size_t ptn$slice$len(const void *self) noexcept;
} // extern "C"

namespace rust
{
    void sliceInit(void *self, const void *ptr, std::size_t len) noexcept {
        ptn$slice$new(self, ptr, len);
    }

    void *slicePtr(const void *self) noexcept
    {
        return ptn$slice$ptr(self);
    }

    std::size_t sliceLen(const void *self) noexcept {
        return ptn$slice$len(self);
    }
}