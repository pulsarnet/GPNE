// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "DockWindowManager.h"
#include "IDockWindowFactory.h"
#include "IDockWindow.h"
#include <corelib/Project.h>
#include <QDockWidget>

/// \brief A static hash table to store dock window factories identified by a unique string key.
static QHash<QString, IDockWindowFactory*> dockWindowFactories;

/// \brief Constructor for DockWindowManager.
/// \param parent The parent QObject.
DockWindowManager::DockWindowManager(QObject* parent) : QObject(parent) {

}

/// \brief Creates all dock windows for a given project.
/// \param project The project for which to create dock windows.
/// \return A list of created dock windows.
QList<IDockWindow*> DockWindowManager::createAllDockWindows(Project* project) {
    QList<IDockWindow*> dockWidgets;
    for (auto [key, factory] : dockWindowFactories.asKeyValueRange()) {
        auto dockWindow = factory->createDockWindow(project);
        dockWidgets.append(dockWindow);
        m_dockWindows.insert(key, dockWindow);
    }
    return dockWidgets;
}

/// \brief Closes all dock windows by clearing the internal storage.
void DockWindowManager::closeAllDockWindows() {
    m_dockWindows.clear();
}

/// \brief Registers a dock window factory with a unique identifier.
/// \param id The unique identifier for the dock window factory.
/// \param factory The dock window factory to register.
void DockWindowManager::registerDockWindowFactory(const QString &id, IDockWindowFactory *factory) {
    if (!factory || id.isEmpty() || dockWindowFactories.contains(id)) {
        return;
    }

    dockWindowFactories.insert(id, factory);
}
