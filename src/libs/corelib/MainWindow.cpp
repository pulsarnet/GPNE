// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "MainWindow.h"
#if defined(Q_OS_WIN32) && !defined(Q_OS_CYGWIN)
#define NOMINMAX
#include <Windows.h>
#endif

#include "Project.h"
#include "ActionManager.h"
#include "ApplicationProjectController.h"
#include "Constants.h"
#include "DockWindowManager.h"
#include "EditorManager.h"
#include "EditorViewManager.h"
#include "IDockWindow.h"
#include "RecentProjectsManager.h"
#include "task/TaskListWidget.h"
#include "task/TaskManager.h"
#include "ui/ProjectListDialog.h"
#include <DockManager.h>
#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QJsonDocument>
#include <QMenuBar>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QStatusBar>

enum Page {
    WelcomePage = 0,
    MainPage = 1,
};

static void initResources()
{
    Q_INIT_RESOURCE(resources);
}

MainWindow::MainWindow()
    : m_taskManager(new TaskManager(this))
    , m_project(nullptr)
{
    initResources();
    connect(m_taskManager, &TaskManager::taskAdded, this, &MainWindow::onTaskAdded);
    connect(m_taskManager, &TaskManager::taskRemoved, this, &MainWindow::onTaskRemoved);

    m_taskListWidget = new TaskListWidget(this);

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    createMenuBar();
    createStatusBar();
    createWelcomePage();
    createEditorViewManager();
    createDockWindowManager();

    this->resize(1280, 720);
    this->setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow() = default;

/**
 * Initialize a project in this window
 */
bool MainWindow::setProject(Project* project)
{
    if (project == m_project) {
        // already opened
        return true;
    }

    for (auto connection : m_connections) {
        disconnect(connection);
    }

    for (auto dockWidget : m_dockWidgets) {
        m_dockManager->removeDockWidget(dockWidget);
    }
    m_dockWidgets.clear();
    m_dockWindowManager->closeAllDockWindows();

    m_project = project;

    if (!m_project) {
        m_stackedWidget->setCurrentIndex(Page::WelcomePage);
        return true;
    }

    m_stackedWidget->setCurrentIndex(Page::MainPage);
    m_connections = {
        connect(
            EditorManager::instance(project),
            &EditorManager::editorOpened,
            m_editorViewManager,
            &EditorViewManager::openEditor
        ),

        connect(
            EditorManager::instance(project),
            &EditorManager::currentEditorChanged,
            m_editorViewManager,
            &EditorViewManager::setCurrentEditor
        ),

        connect(
            EditorManager::instance(project),
            &EditorManager::editorClosed,
            m_editorViewManager,
            &EditorViewManager::closeEditor
        ),
    };

    // load dock widgets
    auto dockWindows = m_dockWindowManager->createAllDockWindows(m_project);
    for (auto dockWindow : dockWindows) {
        auto dockWidget = m_dockManager->createDockWidget(dockWindow->title());
        dockWidget->setWidget(dockWindow->widget());
        dockWidget->setToolBar(dockWindow->toolbar());
        dockWidget->setIcon(dockWindow->icon());
        dockWidget->setFeature(ads::CDockWidget::DockWidgetPinnable, true);
        m_dockManager->addDockWidget(ads::LeftDockWidgetArea, dockWidget);
        m_dockWidgets.append(dockWidget);
    }

    return true;
}


Project* MainWindow::project() const {
    return m_project;
}

void MainWindow::onProjectNameChanged()
{
    this->setWindowTitle(QString("%1[*]").arg(this->m_project->name()));
}

EditorViewManager* MainWindow::editorViewManager() const {
    return m_editorViewManager;
}

TaskManager* MainWindow::taskManager() const {
    return m_taskManager;
}

void MainWindow::onAboutToShowProjects()
{
    auto recentProjectsAction = ActionManager::action(Constants::A_RECENT_PROJECTS);
    auto recentMenu = recentProjectsAction->menu();
    if (!recentMenu) {
        recentMenu = new QMenu();
        recentProjectsAction->setMenu(recentMenu);
    }

    recentMenu->clear();

    const auto& projects = RecentProjectsManager::instance()->getProjects();
    for (const auto& recent : projects) {
        auto action = new QAction(recent.name, this);
        const auto path = recent.path;
        connect(action, &QAction::triggered, this, [this, path]() {
            ApplicationProjectController::openProject(path, this);
        });
        recentMenu->addAction(action);
    }
}

void MainWindow::onAboutToShowDockWindows() {
    auto dockViewAction = ActionManager::action(Constants::A_DOCK_VIEW);
    dockViewAction->menu()->clear();

    for (auto dockWidget : m_dockWidgets) {
        auto action = dockWidget->toggleViewAction();
        action->setIcon(dockWidget->icon());
        dockViewAction->menu()->addAction(action);
    }

}

void MainWindow::onDocumentChanged()
{
    setWindowModified(true);
}

void MainWindow::onTaskAdded(Task* task)
{
    m_taskListWidget->addTask(task);
    m_tasksButton->setText(QString("Running tasks %1").arg(m_taskManager->tasksCount()));
    if (m_tasksButton->isHidden() && m_taskListWidget->isHidden()) {
        m_tasksButton->setVisible(true);
    }
}

void MainWindow::onTaskRemoved(Task* task)
{
    m_taskListWidget->removeTask(task);
    m_tasksButton->setText(QString("Running tasks %1").arg(m_taskManager->tasksCount()));
    if (m_taskManager->tasksCount() == 0) {
        m_tasksButton->hide();
        m_taskListWidget->hide();
    }
}

void MainWindow::onTaskWindowRequested()
{
    if (m_taskListWidget->isHidden()) {
        m_taskListWidget->setVisible(true);
    }
    m_taskListWidget->raise();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug().nospace() << "MainWindow::closeEvent(" << event << ")";
    if (m_project) {
        if (!ApplicationProjectController::closeProject(m_project, false)) {
            event->ignore();
            return;
        }
    }
    event->accept();
}

void MainWindow::createMenuBar()
{
    m_menuBar = new QMenuBar;

    auto fileGroup = ActionManager::group(Constants::FILE_GROUP);
    auto editGroup = ActionManager::group(Constants::EDIT_GROUP);
    auto viewGroup = ActionManager::group(Constants::VIEW_GROUP);
    auto toolsGroup = ActionManager::group(Constants::TOOLS_GROUP);
    auto windowGroup = ActionManager::group(Constants::WINDOW_GROUP);
    auto helpGroup = ActionManager::group(Constants::HELP_GROUP);

    auto fileMenu = fileGroup->createMenu(tr("&File"));
    auto editMenu = editGroup->createMenu(tr("&Edit"));
    auto viewMenu = viewGroup->createMenu(tr("&View"));
    auto toolsMenu = toolsGroup->createMenu(tr("&Tools"));
    auto windowMenu = windowGroup->createMenu(tr("&Window"));
    auto helpMenu = helpGroup->createMenu(tr("&Help"));

    connect(fileMenu, &QMenu::aboutToShow, this, &MainWindow::onAboutToShowProjects);
    connect(viewMenu, &QMenu::aboutToShow, this, &MainWindow::onAboutToShowDockWindows);

    m_menuBar->addMenu(fileMenu);
    m_menuBar->addMenu(editMenu);
    m_menuBar->addMenu(viewMenu);
    m_menuBar->addMenu(toolsMenu);
    m_menuBar->addMenu(windowMenu);
    m_menuBar->addMenu(helpMenu);

    setMenuBar(m_menuBar);
}

void MainWindow::createStatusBar()
{
    m_statusBar = new QStatusBar(this);

    m_tasksButton = new QPushButton;
    m_tasksButton->setChecked(false);
    m_tasksButton->hide();

    connect(m_tasksButton, &QPushButton::clicked, this, &MainWindow::onTaskWindowRequested);

    m_statusBar->addPermanentWidget(m_tasksButton);

    this->setStatusBar(m_statusBar);
}

void MainWindow::createWelcomePage() {
    m_welcomePage = new ProjectListDialog(this);
    m_stackedWidget->addWidget(m_welcomePage);
    m_stackedWidget->setCurrentIndex(Page::WelcomePage);
}

void MainWindow::createEditorViewManager() {
    m_dockManager = new ads::CDockManager();
    m_stackedWidget->addWidget(m_dockManager);
    m_editorViewManager = new EditorViewManager();

    auto centralWidget = m_dockManager->createDockWidget(tr("Editor view"));
    centralWidget->setWidget(m_editorViewManager->widget());
    centralWidget->setFeature(ads::CDockWidget::NoTab, true);
    m_dockManager->setCentralWidget(centralWidget);

}

void MainWindow::createDockWindowManager() {
    m_dockWindowManager = new DockWindowManager();
}
