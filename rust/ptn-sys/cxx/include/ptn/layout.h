// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef LAYOUT_H
#define LAYOUT_H

#include "opaque.h"
#include "util.h"
#include <type_traits>

namespace rust
{
    class layout {
        template <typename T>
        friend std::size_t size_of();
        template <typename T>
        friend std::size_t align_of();
        template <typename T>
        static typename std::enable_if<std::is_base_of<ptn::Opaque, T>::value,
                                       std::size_t>::type
        do_size_of() {
            return T::layout::size();
        }
        template <typename T>
        static typename std::enable_if<!std::is_base_of<ptn::Opaque, T>::value,
                                       std::size_t>::type
        do_size_of() {
            return sizeof(T);
        }
        template <typename T>
        static
            typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
            size_of() {
            return do_size_of<T>();
        }
        template <typename T>
        static typename std::enable_if<std::is_base_of<ptn::Opaque, T>::value,
                                       std::size_t>::type
        do_align_of() {
            return T::layout::align();
        }
        template <typename T>
        static typename std::enable_if<!std::is_base_of<ptn::Opaque, T>::value,
                                       std::size_t>::type
        do_align_of() {
            return alignof(T);
        }
        template <typename T>
        static
            typename std::enable_if<detail::is_complete<T>::value, std::size_t>::type
            align_of() {
            return do_align_of<T>();
        }
    };

    template <typename T>
    std::size_t size_of() {
        return layout::size_of<T>();
    }

    template <typename T>
    std::size_t align_of() {
        return layout::align_of<T>();
    }
}

#endif //LAYOUT_H
