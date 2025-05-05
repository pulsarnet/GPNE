// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROJECTLISTITEM_H
#define PROJECTLISTITEM_H

#include <QFrame>

class MainWindow;

class ProjectListItem : public QFrame {
public:
    ProjectListItem(MainWindow* window, const QString& name, const QString& path, QWidget* parent = nullptr);

    void mousePressEvent(QMouseEvent *event) override;
private:
    MainWindow* m_window;
    QString m_path;
};

#endif //PROJECTLISTITEM_H
