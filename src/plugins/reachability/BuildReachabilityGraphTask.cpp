// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "BuildReachabilityGraphTask.h"
#include <ptn/reachability.h>
#include <thread>

#define DEFAULT_LIMIT 1000
#define DEFAULT_HARD_LIMIT 100'000 // todo settings

BuildReachabilityGraphTask::BuildReachabilityGraphTask(ptn::net::PetriNet* net)
    : Task("Build Reachability Graph")
{
    m_reachability = ptn::modules::reachability::Reachability::init(net, DEFAULT_HARD_LIMIT);
    setCancelable(true);
    setSteps(0);
}

void BuildReachabilityGraphTask::run()
{
    bool complete = true;
    size_t generated = 0;
    do {
        if (isCancel()) {
            complete = false;
            break;
        }

        generated = m_reachability->compute(DEFAULT_LIMIT);
        setMessage(QString("Reached %1 markings").arg(m_reachability->count()));
    } while (generated);

    if (complete) {
        emit graphComplete(m_reachability);
    } else {
        m_reachability->drop();
    }
}