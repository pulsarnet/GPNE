// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_MAINWINDOW_H
#define GPNE_MAINWINDOW_H

#include <QMainWindow>

class QPushButton;

namespace ads {
    class CDockWidget;
    class CDockManager;
}

class IEditor;
class QStackedWidget;
class QStackedLayout;
class QSettings;
class ProjectListDialog;
class TaskListWidget;
class ProgressTaskWidget;
class QMenuBar;
class QAction;
class QToolBar;
class QStatusBar;
class ActionTabWidget;
class ApplicationProjectController;
class EditorViewManager;
class EditorWidget;
class Project;
class TaskManager;
class DockWindowManager;
class Task;


class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    MainWindow();
    MainWindow(const MainWindow&) = delete;
    MainWindow(MainWindow&&) = delete;
    ~MainWindow() override;

    // open project in current window
    bool setProject(Project*);

    Project* project() const;

    void onProjectNameChanged();

    EditorViewManager* editorViewManager() const;

    TaskManager* taskManager() const;

public slots:
    void onAboutToShowProjects();
    void onAboutToShowDockWindows();

    void onDocumentChanged();

    // task
    void onTaskAdded(Task* task);
    void onTaskRemoved(Task* task);

    void onTaskWindowRequested();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void createMenuBar();
    void createStatusBar();
    void createWelcomePage();
    void createEditorViewManager();
    void createDockWindowManager();

    EditorViewManager* m_editorViewManager;
    DockWindowManager* m_dockWindowManager;
    ProjectListDialog* m_welcomePage;

    ads::CDockManager* m_dockManager = nullptr;

    TaskManager* m_taskManager = nullptr;

    // Menu bar
    QMenuBar* m_menuBar = nullptr;
    QStackedWidget* m_stackedWidget = nullptr;
    QStatusBar* m_statusBar = nullptr;

    Project* m_project;

    QPushButton* m_tasksButton;
    TaskListWidget* m_taskListWidget;

    QList<QMetaObject::Connection> m_connections;
    QList<ads::CDockWidget*> m_dockWidgets;

    bool m_closing = false;
};

#endif // GPNE_MAINWINDOW_H
