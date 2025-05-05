// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef DOCKWINDOWMANAGER_H
#define DOCKWINDOWMANAGER_H

#include <QHash>
#include <QObject>

class IDockWindow;
class Project;
class IDockWindowFactory;

class DockWindowManager : public QObject {

    Q_OBJECT

public:

    explicit DockWindowManager(QObject* parent = nullptr);

    QList<IDockWindow*> createAllDockWindows(Project*);
    void closeAllDockWindows();

    static void registerDockWindowFactory(const QString& id, IDockWindowFactory* factory);

private:

    QHash<QString, IDockWindow*> m_dockWindows;

};



#endif //DOCKWINDOWMANAGER_H
