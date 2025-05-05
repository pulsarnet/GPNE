// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ProgressTaskWidget.h"
#include "Task.h"

#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

ProgressTaskWidget::ProgressTaskWidget(Task* task, QWidget* parent)
    : QWidget(parent)
    , m_task(task)
    , m_title(new QLabel)
    , m_text(new QLabel)
    , m_progressBar(new QProgressBar)
    , m_cancelButton(new QPushButton)
{
    m_cancellationToken = m_task->cancellationToken();
    m_cancelButton->setIcon(QIcon(":/images/icons/cancel.svg"));
    m_cancelButton->setFlat(true);
    m_cancelButton->setVisible(m_task->canCancel());
    m_title->setStyleSheet("font-weight: bold;");
    m_title->setText(m_task->title());
    m_text->setText(m_task->message());

    m_progressBar->setTextVisible(false);
    m_progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(m_task->steps());
    m_progressBar->setValue(0);

    connect(m_task, &Task::progressChanged, this, &ProgressTaskWidget::onProgressChanged);
    connect(m_task, &Task::titleChanged, this, &ProgressTaskWidget::onTitleChanged);
    connect(m_task, &Task::messageChanged, this, &ProgressTaskWidget::onMessageChanged);
    connect(m_task, &Task::cancelableChanged, this, &ProgressTaskWidget::onCancelableChanged);
    connect(m_task, &Task::finished, this, &ProgressTaskWidget::onTaskFinished);
    connect(m_cancelButton, &QPushButton::clicked, this, &ProgressTaskWidget::onCancelTask);

    auto layout = new QGridLayout;
    layout->addWidget(m_title, 0, 0, 1, 2, Qt::AlignLeft);
    layout->addWidget(m_text, 1, 0, 1, 2, Qt::AlignLeft);
    layout->addWidget(m_progressBar, 2, 0, 1, 1);
    layout->addWidget(m_cancelButton, 2, 1, 1, 1, Qt::AlignCenter);
    setLayout(layout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void ProgressTaskWidget::onTitleChanged(QString title) { m_title->setText(title); }

void ProgressTaskWidget::onMessageChanged(QString message) { m_text->setText(message); }

void ProgressTaskWidget::onCancelableChanged(bool enabled) { m_cancelButton->setVisible(enabled); }

void ProgressTaskWidget::onTaskFinished() {}

void ProgressTaskWidget::onProgressChanged(int progress) { m_progressBar->setValue(progress); }

void ProgressTaskWidget::onCancelTask() { m_cancellationToken.cancel(); }
