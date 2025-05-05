// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROGRESSTASKWIDGET_H
#define PROGRESSTASKWIDGET_H

#include "CancellationToken.h"
#include <QWidget>

class QLabel;
class QProgressBar;
class QPushButton;
class Task;

class ProgressTaskWidget : public QWidget {

    Q_OBJECT

public:

    explicit ProgressTaskWidget(Task* task, QWidget* parent = nullptr);

public slots:

    void onTitleChanged(QString title);
    void onMessageChanged(QString title);
    void onCancelableChanged(bool enabled);
    void onTaskFinished();
    void onProgressChanged(int progress);
    void onCancelTask();

private:

    Task* m_task;
    QLabel* m_title;
    QLabel* m_text;
    QProgressBar* m_progressBar;
    QPushButton* m_cancelButton;
    CancellationToken m_cancellationToken;
};

#endif //PROGRESSTASKWIDGET_H
