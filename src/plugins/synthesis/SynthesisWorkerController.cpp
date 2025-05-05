// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SynthesisWorkerController.h"

#include <corelib/ApplicationProjectController.h>

#include "SynthesisController.h"
#include <corelib/task/TaskManager.h>

SynthesisWorkerController::SynthesisWorkerController(
    std::shared_ptr<SynthesisContext> context,
    QObject* parent
)
    : QObject(parent)
    , m_context(context)
    , m_running(false)
    , m_abort(false)
{}

SynthesisWorkerController::~SynthesisWorkerController()
{
    abort();
}

void SynthesisWorkerController::start()
{
    if (m_running) {
        return;
    }

    SynthesisWorker* synthesisWorker = new SynthesisWorker(m_context);
    m_synthesisTaskToken = synthesisWorker->cancellationToken();

    connect(
        synthesisWorker,
        &SynthesisWorker::programsEvaluated,
        this,
        &SynthesisWorkerController::onProgramsEvaluated,
        Qt::QueuedConnection
    );

    ApplicationProjectController::runBackgroundTask(ApplicationProjectController::currentProject(), synthesisWorker);
}

void SynthesisWorkerController::abort()
{
    m_abort = true;
    m_synthesisTaskToken.cancel();
}

bool SynthesisWorkerController::isRunning() const noexcept { return m_running; }

bool SynthesisWorkerController::isAborted() const noexcept
{
    return m_abort;
}

void SynthesisWorkerController::onProgramsEvaluated(const QList<Evaluated>& array)
{
    if (m_abort) {
        return;
    }

    emit programsEvaluated(array);
}