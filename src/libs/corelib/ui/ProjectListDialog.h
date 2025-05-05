// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROJECTLISTDIALOG_H
#define PROJECTLISTDIALOG_H

#include <QWidget>

class ProjectListItem;
class MainWindow;
class QVBoxLayout;
class QScrollArea;
class QToolButton;

class ProjectListDialog : public QWidget {

    Q_OBJECT

public:
    ProjectListDialog(MainWindow* parent);

public slots:
    void onProjectsChanged();

signals:
    void createProject();
    void openProject();

private:
    MainWindow* m_mainWindow;

    QToolButton* m_createProjectButton;
    QToolButton* m_openProjectButton;

    QVBoxLayout* m_projectsLayout;
    QScrollArea* m_scrollArea;
    QList<ProjectListItem*> m_projectListItems;
};



#endif //PROJECTLISTDIALOG_H
