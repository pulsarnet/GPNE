// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef TASK_H
#define TASK_H

#include "CancellationToken.h"
#include <QObject>

class Task : public QObject {

    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(bool cancelable READ canCancel WRITE setCancelable NOTIFY cancelableChanged)
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)

    Q_DISABLE_COPY_MOVE(Task);

public:

    explicit Task(const QString& title);

    void exec();

    bool canCancel() const;

    CancellationToken cancellationToken() { return m_cancelToken; }

    const QString& title() const;
    const QString& message() const;

    void setSteps(int steps);
    int steps() const noexcept;

signals:

    void titleChanged(QString title);
    void messageChanged(QString message);
    void progressChanged(int);
    void hasProgressChanged(bool);
    void cancelableChanged(bool);
    void finished();

protected:

    virtual void run() = 0;

    void setTitle(const QString& title);

    void setMessage(const QString& message);

    int progress() const;
    void setProgress(int i);

    void setCancelable(bool enabled);

    bool isCancel();

private:
    CancellationToken m_cancelToken;
    QAtomicInt m_cancelable;

    QString m_text;
    QString m_title;
    int m_progress;

    int m_steps = 0;
};

#endif //TASK_H
