// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef CONDITION_H
#define CONDITION_H

#include <QMetaType>

namespace ptn::net
{
    struct PetriNet;
}

struct Range {
    std::optional<int> start;
    std::optional<int> end;

    bool operator==(const Range& rsh) const { return start == rsh.start && end == rsh.end; }

    bool isMatch(int value) const noexcept;
    bool isFullUnbound() const noexcept;

    int diff(int value) const noexcept;
};
Q_DECLARE_METATYPE(Range)

#endif // CONDITION_H
