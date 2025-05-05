// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef DECOMPOSETASK_H
#define DECOMPOSETASK_H

#include <corelib/task/Task.h>

namespace ptn::modules::decompose
{
    struct DecomposeContext;
}

namespace ptn::net
{
    struct PetriNet;
}

class DecomposeTask : public Task {

    Q_OBJECT

public:

    using decomposition_t = ptn::modules::decompose::DecomposeContext;
    using shared_return_t = std::shared_ptr<decomposition_t>;
    using net_t = ptn::net::PetriNet;
    using shared_net_t = std::shared_ptr<net_t>;

    DecomposeTask(net_t* net);

signals:

    void decompositionReady(shared_return_t decomposition);

protected:

    void run() override;

private:

    shared_net_t m_net;

};

#endif //DECOMPOSETASK_H
