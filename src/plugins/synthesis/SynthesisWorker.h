// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SYNTHESISWORKER_H
#define SYNTHESISWORKER_H

#include <corelib/task/Task.h>

class Condition;
class Project;
class AbstractAnalysisModule;

namespace ptn::net
{
    struct PetriNet;
}

namespace ptn::modules::decompose
{
    struct DecomposeContext;
}

struct Evaluated {
    size_t idx;
    std::vector<uint16_t> places;
    std::vector<uint16_t> transitions;
    std::vector<QVariant> metrics;
};

struct SynthesisContext {
    ptn::modules::decompose::DecomposeContext* decomposition;
    std::vector<std::shared_ptr<AbstractAnalysisModule>> analysisModules;
};

class SynthesisWorker : public Task {

    Q_OBJECT

public:
    SynthesisWorker(std::shared_ptr<SynthesisContext> context);
    ~SynthesisWorker();

public slots:

    void run() override;

signals:
    void programsEvaluated(const QList<Evaluated>& evaluated);

private:

    std::shared_ptr<SynthesisContext> m_context;
    std::vector<int> m_preAnalysisModules;
    std::vector<int> m_postAnalysisModules;
    std::vector<std::pair<size_t, size_t>> m_moduleToMetricStorageRange;
    size_t m_totalMetrics;
};

#endif // SYNTHESISWORKER_H
