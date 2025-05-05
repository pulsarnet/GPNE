// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Core.h"
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPluginLoader>
#include <QtPlugin>
#include "ActionManager.h"
#include "ApplicationProjectController.h"
#include "Constants.h"
#include "MainWindow.h"
#include "PluginInterface.h"
#include "Project.h"
#include "RecentProjectsManager.h"

Q_IMPORT_PLUGIN(EditorPlugin)
Q_IMPORT_PLUGIN(SimulationPlugin)
Q_IMPORT_PLUGIN(ProjectExplorerPlugin)
Q_IMPORT_PLUGIN(MatrixPlugin)
Q_IMPORT_PLUGIN(SynthesisPlugin)
Q_IMPORT_PLUGIN(ReachabilityPlugin)

void Core::initialize() {

    QObject::connect(qApp, &QApplication::focusChanged, ApplicationProjectController::instance(), &ApplicationProjectController::onFocusChanged);

    QAction* newProjectAction = new QAction(QIcon(Constants::ICON_CREATE), QObject::tr("New Project"));
    QAction* openProjectAction = new QAction(QIcon(Constants::ICON_FOLDER), QObject::tr("Open Project"));
    openProjectAction->setShortcut(QKeySequence::Open);

    QAction* recentProjectsAction = new QAction(QObject::tr("Recent Projects"));
    QAction* saveProjectAction = new QAction(QIcon(Constants::ICON_SAVE_PROJECT), QObject::tr("Save Project"));
    QAction* closeProjectAction = new QAction(QIcon(Constants::ICON_CLOSE_PROJECT), QObject::tr("Close Project"));
    QAction* exitAction = new QAction(QIcon(Constants::ICON_EXIT), QObject::tr("Exit"));
    QAction* aboutAction = new QAction(QIcon(Constants::ICON_ABOUT), QObject::tr("About"));
    QAction* aboutQtAction = new QAction(QIcon(Constants::ICON_ABOUT), QObject::tr("About Qt"));
    QAction* dockViewAction = new  QAction(QObject::tr("Dock windows"));
    dockViewAction->setMenu(new QMenu());

    QObject::connect(newProjectAction, &QAction::triggered, []() {
        ApplicationProjectController::createProject(); // create project in current window
    });

    QObject::connect(openProjectAction, &QAction::triggered, []() {
        ApplicationProjectController::openProject(); // open project in current window
    });

    QObject::connect(saveProjectAction, &QAction::triggered, []() {
        ApplicationProjectController::saveProject(); // open project in current window
    });

    QObject::connect(closeProjectAction, &QAction::triggered, []() {
        ApplicationProjectController::closeProject();
    });

    QObject::connect(aboutAction, &QAction::triggered, []() {
        ApplicationProjectController::about();
    });

    QObject::connect(aboutQtAction, &QAction::triggered, []() {
        QApplication::aboutQt();
    });

    connect(exitAction, &QAction::triggered, []() {
        ApplicationProjectController::closeApplication();
    });

    ActionManager::registerAction(Constants::A_NEW_PROJECT, newProjectAction);
    ActionManager::registerAction(Constants::A_OPEN_PROJECT, openProjectAction);
    ActionManager::registerAction(Constants::A_RECENT_PROJECTS, recentProjectsAction);
    ActionManager::registerAction(Constants::A_SAVE_PROJECT, saveProjectAction);
    ActionManager::registerAction(Constants::A_CLOSE_PROJECT, closeProjectAction);
    ActionManager::registerAction(Constants::A_EXIT, exitAction);
    ActionManager::registerAction(Constants::A_ABOUT, aboutAction);
    ActionManager::registerAction(Constants::A_ABOUT_QT, aboutQtAction);
    ActionManager::registerAction(Constants::A_DOCK_VIEW, dockViewAction);

    auto fileGroup = ActionManager::createActionGroup(Constants::FILE_GROUP);
    fileGroup->addAction(Constants::A_NEW_PROJECT);
    fileGroup->addAction(Constants::A_OPEN_PROJECT);
    fileGroup->addAction(Constants::A_RECENT_PROJECTS);
    fileGroup->addAction(Constants::A_CLOSE_PROJECT);
    fileGroup->addSeparator();
    fileGroup->addAction(Constants::A_SAVE_PROJECT);
    fileGroup->addAction(Constants::A_EXIT);

    ActionManager::createActionGroup(Constants::EDIT_GROUP);

    auto viewGroup = ActionManager::createActionGroup(Constants::VIEW_GROUP);
    viewGroup->addAction(Constants::A_DOCK_VIEW);
    viewGroup->addSeparator();

    ActionManager::createActionGroup(Constants::TOOLS_GROUP);
    ActionManager::createActionGroup(Constants::WINDOW_GROUP);

    auto helpGroup = ActionManager::createActionGroup(Constants::HELP_GROUP);
    helpGroup->addAction(Constants::A_ABOUT);
    helpGroup->addAction(Constants::A_ABOUT_QT);

    const auto staticInstances = QPluginLoader::staticInstances();
    for (QObject* plugin : staticInstances) {
        auto pluginInterface = qobject_cast<Plugin::PluginInterface*>(plugin);
        if (pluginInterface) {
            pluginInterface->initialize();
        }
    }

    connect(ApplicationProjectController::instance(), &ApplicationProjectController::projectOpened, [](Project* project) {
        RecentProjectsManager::instance()->addProject(project->name(), QString::fromStdString(project->filePath().string()));
    });

    Core::startApplication();
}

void Core::startApplication() {
    ApplicationProjectController::createMainWindow()->show();
}
