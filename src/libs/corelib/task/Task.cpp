// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Task.h"

Task::Task(const QString& title)
    : QObject(nullptr)
    , m_cancelable(true)
    , m_title(title)
    , m_text(QString())
    , m_progress(-1)
{}

void Task::exec() {
    run();
    emit finished();
}

bool Task::canCancel() const { return m_cancelable.loadAcquire(); }

const QString& Task::title() const { return m_title; }

const QString& Task::message() const { return m_text; }

void Task::setSteps(const int steps) { m_steps = steps; }

int Task::steps() const noexcept { return m_steps; }

void Task::setTitle(const QString& title)
{
    if (m_title == title) {
        return;
    }

    m_title = title;
    emit titleChanged(m_title);
}

void Task::setMessage(const QString& message)
{
    if (m_text == message) {
        return;
    }

    m_text = message;
    emit messageChanged(m_text);
}

int Task::progress() const { return m_progress; }

void Task::setProgress(int i)
{
    if (m_progress == i || i > m_steps || i < 0) {
        return;
    }

    m_progress = i;
    emit progressChanged(i);
}

void Task::setCancelable(bool enabled)
{
    bool old = m_cancelable.fetchAndStoreAcquire(enabled);
    if (old == enabled) {
        return;
    }

    emit cancelableChanged(enabled);
}

bool Task::isCancel() { return (bool)m_cancelToken; }
