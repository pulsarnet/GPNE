// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QHash>
#include <QObject>

class Task;

class TaskManager : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TaskManager);

public:
    explicit TaskManager(QObject* parent = nullptr);

    void spawnTask(Task* task);

    int tasksCount() const;

    void abortAllTasks();

signals:

    void taskAdded(Task* task);
    void taskRemoved(Task* task);

protected:
    void onTaskFinished(Task* task);

private:
    QHash<Task*, QThread*> m_tasks;
};

#endif // TASKMANAGER_H
