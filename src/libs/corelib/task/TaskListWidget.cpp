// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "TaskListWidget.h"
#include "ProgressTaskWidget.h"

#include <QScrollArea>
#include <QVBoxLayout>

TaskListWidget::TaskListWidget(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::CustomizeWindowHint);
    setWindowTitle(tr("Running tasks"));

    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(QMargins());
    m_layout->addStretch(10);

    QWidget* w = new QWidget;
    w->setLayout(m_layout);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(w);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);

    setLayout(new QVBoxLayout);
    layout()->setContentsMargins(QMargins());
    layout()->addWidget(scrollArea);

    setMinimumWidth(400);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setContentsMargins(QMargins());
}

void TaskListWidget::addTask(Task* task)
{
    auto progressWidget = new ProgressTaskWidget(task);
    m_tasks.insert(task, progressWidget);
    m_layout->insertWidget(0, progressWidget);
}

void TaskListWidget::removeTask(Task* task)
{
    if (m_tasks.contains(task)) {
        QWidget* widget = m_tasks.value(task);
        m_layout->removeWidget(widget);
        widget->deleteLater();

        m_tasks.remove(task);
    }
}
