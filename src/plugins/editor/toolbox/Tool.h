// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_TOOL_H
#define GPNE_TOOL_H

#include <QToolButton>

class Tool : public QToolButton {

public:
    explicit Tool(QAction* action, QWidget* parent = nullptr);
    explicit Tool(QMenu* action, QWidget* parent = nullptr);

    void setSize(const QSize& size);

    void paintEvent(QPaintEvent*) override;

protected slots:

    void onTriggered(QAction* action);

protected:
    bool event(QEvent* e) override;

    void enterEvent(QEnterEvent*) override;

    void leaveEvent(QEvent*) override;

    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // GPNE_TOOL_H
