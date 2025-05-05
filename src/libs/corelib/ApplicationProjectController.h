// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_APPLICATIONPROJECTCONTROLLER_H
#define GPNE_APPLICATIONPROJECTCONTROLLER_H

#include <unordered_map>
#include <QObject>

class IEditor;
class Project;
class MainWindow;
class Task;

class ApplicationProjectController : public QObject {

    Q_OBJECT

public:
    static ApplicationProjectController* instance();

    ApplicationProjectController(const ApplicationProjectController&) = delete;
    ApplicationProjectController(ApplicationProjectController&&) = delete;

    static MainWindow* createMainWindow();
    static MainWindow* currentMainWindow();
    static void runBackgroundTask(Project* project, Task* task);

    static void createProject();
    static void createProject(MainWindow* callsite);
    static void createProject(const QString& projectName, const QString& dirPath);

    static void openProject();
    static void openProject(MainWindow* callsite);
    static void openProject(const QString &projectPath, MainWindow* window);

    static void saveProject();

    static bool closeProject(bool closeWindow = false);
    static bool closeProject(Project* window, bool closeWindow = false);

    static void closeApplication();

    static Project* currentProject();

    static void about();

    static MainWindow* getProjectWindowIfOpened(const QString& projectPath);
    static MainWindow* getProjectWindow(Project*);

    static std::unordered_map<MainWindow*, Project*>& openedProjects();

public slots:

    void onFocusChanged(QWidget* old, QWidget* now);

signals:

    void currentWindowChanged(MainWindow* old, MainWindow* now);
    void projectOpened(Project* project);
    void projectClosed(Project* project);
    void editorOpened(IEditor* editor);
    void editorClosed(IEditor* editor);

private:

    ApplicationProjectController() = default;

    std::unordered_map<MainWindow*, Project*> m_openedProjects;
    MainWindow* m_currentWindow = nullptr;

};

#endif // GPNE_APPLICATIONPROJECTCONTROLLER_H
