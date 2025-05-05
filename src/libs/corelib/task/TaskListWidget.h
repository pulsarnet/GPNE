// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef TASKLISTWIDGET_H
#define TASKLISTWIDGET_H

#include <QWidget>

class QVBoxLayout;
class Task;
class ProgressTaskWidget;

class TaskListWidget : public QWidget {

    Q_OBJECT

public:
    explicit TaskListWidget(QWidget* parent = nullptr);

    void addTask(Task* task);
    void removeTask(Task* task);

private:
    QHash<Task*, QWidget*> m_tasks;
    QVBoxLayout* m_layout;
};

#endif // TASKLISTWIDGET_H
