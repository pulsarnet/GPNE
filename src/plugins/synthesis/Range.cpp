// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Range.h"

bool Range::isMatch(int value) const noexcept
{
    return (!start.has_value() || start.value() <= value) &&
           (!end.has_value() || end.value() >= value);
}

bool Range::isFullUnbound() const noexcept
{
    return !start.has_value() && !end.has_value();
}

int Range::diff(int value) const noexcept {
    if (start.has_value() && value < start.value()) {
        return start.value() - value;
    }

    if (end.has_value() && value > end.value()) {
        return value - end.value();
    }

    return 0;
}
