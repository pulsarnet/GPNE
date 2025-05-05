// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "DecomposeTask.h"

#include <QDebug>
#include <ptn/decompose.h>
#include <ptn/net.h>

DecomposeTask::DecomposeTask(net_t* net) : Task(tr("Decompose task")) {
    m_net = std::shared_ptr<net_t>(net_t::clone(net), &net_t::drop);
}

void DecomposeTask::run() {
    auto decomposition = decomposition_t::init(m_net.get());
    auto shared = std::shared_ptr<decomposition_t>(decomposition, &decomposition_t::drop);
    emit decompositionReady(shared);
}
