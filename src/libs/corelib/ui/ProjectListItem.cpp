// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ProjectListItem.h"
#include <corelib/ApplicationProjectController.h>
#include <QGridLayout>
#include <QMouseEvent>
#include <QLabel>

ProjectListItem::ProjectListItem(MainWindow* window, const QString& name, const QString& path, QWidget *parent) : QFrame(parent), m_window(window), m_path(path) {
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setFrameShape(QFrame::StyledPanel);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setCursor(Qt::PointingHandCursor);

    QGridLayout* projectLayout = new QGridLayout(this);
    QLabel* projectName = new QLabel(name, this);
    projectName->setStyleSheet("font-weight: bold; color: blue;");
    projectLayout->addWidget(projectName);
    projectLayout->addWidget(new QLabel(path, this), 1, 0);
    setLayout(projectLayout);
    setObjectName("hellow");
    setStyleSheet("#hellow:hover { background-color: #eeeeee; }");
}

void ProjectListItem::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        ApplicationProjectController::openProject(m_path, m_window);
    }
    QFrame::mousePressEvent(event);
}