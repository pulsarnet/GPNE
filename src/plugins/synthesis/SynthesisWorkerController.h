// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SYNTHESISWORKERCONTROLLER_H
#define SYNTHESISWORKERCONTROLLER_H

#include "SynthesisWorker.h"
#include <QObject>

class TaskManager;

class SynthesisWorkerController : public QObject {

    Q_OBJECT

public:
    SynthesisWorkerController(std::shared_ptr<SynthesisContext> context, QObject* parent = nullptr);
    ~SynthesisWorkerController();

    void start();
    void abort();

    bool isRunning() const noexcept;
    bool isAborted() const noexcept;

public slots:
    void onProgramsEvaluated(const QList<Evaluated>& array);

signals:
    void programsEvaluated(const QList<Evaluated>& array);

private:
    std::shared_ptr<SynthesisContext> m_context;

    CancellationToken m_synthesisTaskToken;

    bool m_running;
    bool m_abort;
};



#endif //SYNTHESISWORKERCONTROLLER_H
