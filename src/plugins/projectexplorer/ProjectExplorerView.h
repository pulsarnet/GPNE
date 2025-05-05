// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROJECTEXPLORERVIEW_H
#define PROJECTEXPLORERVIEW_H

#include <corelib/IDockWindow.h>
#include <corelib/IDockWindowFactory.h>

class Project;

class ProjectExplorerView : public IDockWindow {
public:
    explicit ProjectExplorerView(Project* project);
    virtual ~ProjectExplorerView() = default;

    QString title() override;
    QIcon icon() override;
    QToolBar* toolbar() override;
    QWidget* widget() override;

private:
    Project* m_project;
};

class ProjectExplorerViewFactory : public IDockWindowFactory {
public:

    IDockWindow *createDockWindow(Project *) override;
};

#endif //PROJECTEXPLORERVIEW_H
