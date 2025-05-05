// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef BUILDREACHABILITYGRAPHTASK_H
#define BUILDREACHABILITYGRAPHTASK_H

#include <corelib/task/Task.h>

namespace ptn::modules::reachability
{
    struct Reachability;
}

namespace ptn::net
{
    struct PetriNet;
}

class BuildReachabilityGraphTask : public Task {

    Q_OBJECT

public:
    BuildReachabilityGraphTask(ptn::net::PetriNet* net); // todo pass copy of net. And delete after
    ~BuildReachabilityGraphTask() = default;

public slots:
    void run() override;

signals:
    void graphComplete(ptn::modules::reachability::Reachability* r);

private:
    ptn::modules::reachability::Reachability* m_reachability;
};

#endif // BUILDREACHABILITYGRAPHTASK_H
