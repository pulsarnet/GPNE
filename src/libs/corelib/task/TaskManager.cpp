// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "TaskManager.h"

#include <qeventloop.h>

#include "Task.h"
#include <QThread>

TaskManager::TaskManager(QObject* parent) : QObject(parent) {}

void TaskManager::spawnTask(Task* task)
{
    if (m_tasks.contains(task)) {
        return;
    }

    QThread* thread = new QThread();
    task->moveToThread(thread);

    m_tasks.insert(task, thread);

    connect(thread, &QThread::started, task, &Task::exec);
    connect(task, &Task::finished, thread, &QThread::quit);
    connect(task, &Task::finished, [this, task]() { onTaskFinished(task); });
    connect(thread, &QThread::finished, task, &Task::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    emit taskAdded(task);
    thread->start();
}

int TaskManager::tasksCount() const { return m_tasks.size(); }

void TaskManager::abortAllTasks() {
    if (m_tasks.isEmpty()) {
        return;
    }

    for (auto [task, thread] : m_tasks.asKeyValueRange()) {
        // try to cancel task
        task->cancellationToken().cancel();
        // try to interrupt thread
        thread->requestInterruption();
        // try to quit thread
        thread->quit();
    }

    for (auto thread : m_tasks.values()) {
        thread->wait();
    }

    m_tasks.clear();
}

void TaskManager::onTaskFinished(Task* task)
{
    m_tasks.remove(task);
    emit taskRemoved(task);
}
