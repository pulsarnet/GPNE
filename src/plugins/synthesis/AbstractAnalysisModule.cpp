// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "AbstractAnalysisModule.h"

void AbstractAnalysisModule::enable(bool enabled) noexcept {
    m_enabled = !canDisable() || enabled;
}

bool AbstractAnalysisModule::isEnabled() const noexcept {
    return m_enabled;
}

bool AbstractAnalysisModule::canDisable() const noexcept {
    return true;
}

bool AbstractAnalysisModule::needValidation() const noexcept {
    return m_enabled && isNeedValidation();
}
