// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef UTIL_H
#define UTIL_H

#include <type_traits>

namespace rust {
    namespace detail {
        namespace {
            template <typename T, typename = std::size_t>
            struct is_complete : std::false_type {};
            template <typename T>
            struct is_complete<T, decltype(sizeof(T))> : std::true_type {};
        } // namespace
    } // namespace detail
}

#endif //UTIL_H
