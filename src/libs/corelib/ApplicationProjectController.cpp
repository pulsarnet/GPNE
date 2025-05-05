// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <config.h>
#include <QApplication>
#include <QFileDialog>
#include <qjsondocument.h>
#include <QJsonObject>
#include <QProgressDialog>
#include <QMessageBox>

#include "ApplicationProjectController.h"
#include "Project.h"
#include "MainWindow.h"
#include "EditorManager.h"
#include "ProjectUtil.h"
#include "Settings.h"
#include "task/TaskManager.h"
#include "ui/NewProjectWindow.h"


const QString& aboutText();

/// \brief Retrieves the singleton instance of ApplicationProjectController.
/// \return A pointer to the singleton instance of ApplicationProjectController.
ApplicationProjectController* ApplicationProjectController::instance() {
    static ApplicationProjectController instance;
    return &instance;
}

/// \brief Creates a new MainWindow instance.
/// \return A pointer to the newly created MainWindow instance.
MainWindow* ApplicationProjectController::createMainWindow()
{
    const auto window = new MainWindow();
    window->setMinimumSize(QSize(1280, 720));
    return window;
}

/// \brief Retrieves the current MainWindow instance.
/// \return A pointer to the current MainWindow instance.
MainWindow* ApplicationProjectController::currentMainWindow() {
    return instance()->m_currentWindow;
}

/// \brief Runs a background task for the specified project.
/// \param project The project for which the task will be run.
/// \param task The task to be run.
void ApplicationProjectController::runBackgroundTask(Project* project, Task* task) {
    if (!task) {
        return;
    }

    MainWindow* window = project ? getProjectWindow(project) : currentMainWindow();
    if (const auto manager = window ? window->taskManager() : nullptr) {
        manager->spawnTask(task);
    } else {
        qFatal() << "No task manager found for project";
    }
}

/// \brief Creates a new project using a file dialog.
/// If no active window is found, a new MainWindow is created.
void ApplicationProjectController::createProject() {
    // FIXME: maybe use the existing window
    auto mainWindow = qobject_cast<MainWindow*>(QApplication::activeWindow());
    if (!mainWindow) {
        mainWindow = createMainWindow();
    }
    createProject(mainWindow);
}

/// \brief Creates a new project using the specified MainWindow as the callsite.
/// \param mainWindow The MainWindow instance from which the project creation is initiated.
void ApplicationProjectController::createProject(MainWindow *mainWindow) {
    NewProjectWindow newProjectDialog(mainWindow);
    if (newProjectDialog.exec() == QDialog::Accepted) {
        openProject(ProjectUtil::getProjectFilePath(newProjectDialog.projectDirPath()), mainWindow);
    }
}

/// \brief Creates a new project in the specified directory.
/// \param projectName The name of the new project.
/// \param parentDirPath The path to the directory where the project will be created.
void ApplicationProjectController::createProject(const QString &projectName, const QString& parentDirPath) {
    // Absolute path of new project directory
    QString projectDirPath = ProjectUtil::canonicalPath(parentDirPath + "/" + projectName);
    if (ProjectUtil::isExistingProjectDirectory(projectDirPath)) {
        qDebug() << "Project already exists:" << projectDirPath;
        return;
    }

    // Create directory
    QDir parentDir(parentDirPath);
    if (!parentDir.mkpath(projectDirPath)) {
        qDebug() << "Failed to create the directory:" << projectDirPath;
        return;
    }

    ProjectData data {
        .id = QUuid::createUuid().toString(),
        .name = projectName,
        .created = QDateTime::currentDateTime(),
    };

    const auto result = persistence::writeProjectData(data, ProjectUtil::getProjectFilePath(projectDirPath).toStdString());
    if (!result) {
        qDebug() << "Failed to initialize the project structure:" << projectDirPath;
        QDir(projectDirPath).removeRecursively();
    }
}

/// \brief Opens a project using a file dialog.
/// If no active window is found, a new MainWindow is created.
void ApplicationProjectController::openProject() {
    auto callsite = qobject_cast<MainWindow*>(QApplication::activeWindow());
    if (!callsite) {
        callsite = createMainWindow();
    }
    openProject(callsite);
}

/// \brief Opens a project file using a file dialog.
///
/// This function displays a file dialog to the user to select a project file
/// with the extension .gpneproj. If a file is selected, it attempts to open
/// the project and updates the last opened project directory setting.
/// \param callsite A pointer to the MainWindow instance from which the file dialog is called.
void ApplicationProjectController::openProject(MainWindow *callsite) {
    QString filename = QFileDialog::getOpenFileName(
        callsite,
        QObject::tr("Open Project"),
        Settings::lastOpenedProjectDirectory(),
        QObject::tr("GPNE project (*.gpneproj)")
    );
    if (filename.isEmpty()) {
        return;
    }

    openProject(filename, callsite);
}

/// \brief Opens project in a new or existing empty window.
/// \param path The absolute path to the project file or directory containing the project file.
/// \param window The window where the project will be opened.
void ApplicationProjectController::openProject(const QString &path, MainWindow* mainWindow)
{
    const auto projectFilePath = ProjectUtil::getProjectFilePath(path);
    if (auto openedProjectWindow = getProjectWindowIfOpened(projectFilePath)) {
        openedProjectWindow->raise();
        return;
    }

    auto currentMainWindow = mainWindow;
    if (mainWindow->project()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QObject::tr("Open Project"));
        msgBox.setStandardButtons(QMessageBox::NoButton);
        QPushButton* thisWindow = msgBox.addButton(QObject::tr("This window"), QMessageBox::YesRole);
        QPushButton* newWindow = msgBox.addButton(QObject::tr("New window"), QMessageBox::NoRole);
        QPushButton* cancelWindow = msgBox.addButton(QMessageBox::Cancel);
        msgBox.setText(QObject::tr("Open in new window?"));
        msgBox.exec();

        if ((QPushButton*)msgBox.clickedButton() == thisWindow) {
            if (!closeProject(mainWindow->project(), false)) {
                return;
            }
        } else if ((QPushButton*)msgBox.clickedButton() == newWindow) {
            mainWindow = createMainWindow();
        } else if ((QPushButton*)msgBox.clickedButton() == cancelWindow) {
            return;
        }
    }

    auto result = Project::load(projectFilePath.toStdString());
    if (!result) {
        qDebug() << "Failed to load project:" << result.error();
        return;
    }

    auto project = result.value().release();
    connect(
        EditorManager::instance(project),
        &EditorManager::editorOpened,
        instance(),
        &ApplicationProjectController::editorOpened
    );

    connect(
        EditorManager::instance(project),
        &EditorManager::editorClosed,
        instance(),
        &ApplicationProjectController::editorClosed
    );

    // Assign the project to the main window here because the window maybe used for the closed project
    openedProjects()[mainWindow] = project;
    if (!mainWindow->setProject(project)) {
        openedProjects().erase(mainWindow);
        if (mainWindow != currentMainWindow) {
            mainWindow->deleteLater();
        }
        return;
    }

    mainWindow->show();
    mainWindow->raise();

    emit instance()->projectOpened(project);
}

void ApplicationProjectController::saveProject() {
    // TODO
}

/// \brief Closes the current project.
/// \param closeWindow If true, the window will also be closed.
/// \return True if the project was successfully closed, false otherwise.
bool ApplicationProjectController::closeProject(bool closeWindow) {
    return closeProject(currentProject(), closeWindow);
}

/// \brief Closes the specified project.
/// \param project The project to be closed.
/// \param closeWindow If true, the window will also be closed.
/// \return True if the project was successfully closed, false otherwise.
bool ApplicationProjectController::closeProject(Project *project, bool closeWindow) {
    // On close project we must save state of this project. Save gui state is on AppData as relative to project file
    // IDocuments saves relative to files

    // get main window of project
    auto window = getProjectWindow(project);

    // If there are unfinished tasks, ask the user if they want to close the project
    if (window->taskManager()->tasksCount() > 0) {
        qDebug() << "There are unfinished tasks. Ask about closing the project";
        QMessageBox msgBox;
        msgBox.setWindowTitle(QObject::tr("Close Project"));
        msgBox.setText(QObject::tr("There are unfinished tasks. Do you want to close the project?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        connect(window->taskManager(), &TaskManager::taskRemoved, &msgBox, [window, &msgBox](Task* task) {
            Q_UNUSED(task);
            if (window->taskManager()->tasksCount() == 0) {
                msgBox.accept();
            }
        });
        if (msgBox.exec() == QMessageBox::No) {
            return false;
        }

        QProgressDialog dialog;
        dialog.setParent(window);
        dialog.setLabelText(QObject::tr("Closing project..."));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.setAutoClose(true);
        dialog.setMaximum(0);
        dialog.setMinimum(0);

        // wait abort all tasks
        window->taskManager()->abortAllTasks();

        // TODO write test plugin which runs infinite task, task with interruption, task with cancellation, blocked task
        // Process all events before closing the project
        QCoreApplication::processEvents();
    }

    // unload documents
    if (!EditorManager::unloadAllEditorsAndDocuments(project)) {
        return false;
    }

    if (closeWindow) {
        if (window) {
            window->close();
        }
    } else {
        if (window) {
            window->setProject(nullptr);
        }
    }

    // remove project from opened projects
    auto instance = ApplicationProjectController::instance();
    auto it = instance->m_openedProjects.find(window);
    if (it != instance->m_openedProjects.end()) {
        instance->m_openedProjects.erase(it);
    }

    emit instance->projectClosed(project);
    delete project;
    return true;
}

void ApplicationProjectController::closeApplication() {
    // Try close all projects
    for (auto& [_, project] : openedProjects()) {
        if (!closeProject(project, true)) {
            return;
        }
    }

    QApplication::quit();
}

/// \brief Retrieves the current project.
/// \return A pointer to the current project.
Project* ApplicationProjectController::currentProject() {
    auto instance = ApplicationProjectController::instance();
    if (!instance->m_currentWindow) {
        return nullptr;
    }

    return instance->m_currentWindow->project();
}

/// \brief Displays the about dialog for the application.
void ApplicationProjectController::about() {
    QMessageBox::about(
        QApplication::activeWindow(),
        tr("About %1").arg(qApp->applicationDisplayName()),
        aboutText()
    );
}

/// \brief Retrieves the MainWindow instance for the specified project file path if it is already opened.
/// \param projectFilePath The file path of the project.
/// \return A pointer to the MainWindow instance if the project is already opened, nullptr otherwise.
MainWindow* ApplicationProjectController::getProjectWindowIfOpened(const QString &projectFilePath) {
    auto it = std::find_if(openedProjects().begin(), openedProjects().end(),
                   [projectFilePath](auto& pair) {
                       return QString::fromStdString(pair.second->filePath().string()) == projectFilePath;
                   });
    return it != openedProjects().end() ? it->first : nullptr;
}

/// \brief Retrieves the MainWindow instance for the specified project.
/// \param project The project for which to retrieve the MainWindow instance.
/// \return A pointer to the MainWindow instance for the specified project.
MainWindow* ApplicationProjectController::getProjectWindow(Project* project) {
    auto instance = ApplicationProjectController::instance();
    auto it = std::find_if(instance->m_openedProjects.begin(), instance->m_openedProjects.end(),
                   [project](auto& pair) { return pair.second == project; });
    return it != instance->m_openedProjects.end() ? it->first : nullptr;
}

/// \brief Retrieves the map of opened projects.
/// \return A reference to the map of opened projects.
std::unordered_map<MainWindow*, Project*>& ApplicationProjectController::openedProjects()
{
    return instance()->m_openedProjects;
}

/// \brief Handles focus change events to update the current MainWindow instance.
/// \param old The previously focused widget.
/// \param now The currently focused widget.
void ApplicationProjectController::onFocusChanged(QWidget *old, QWidget *now) {
    Q_UNUSED(old)
    if (m_currentWindow && (m_currentWindow == now || m_currentWindow->isAncestorOf(now))) {
        return;
    }

    auto oldCurrentWindow = m_currentWindow;
    if (now) {
        while (now->parentWidget()) {
            now = now->parentWidget();
        }

        m_currentWindow = qobject_cast<MainWindow*>(now);
    } else {
        m_currentWindow = nullptr;
    }

    emit currentWindowChanged(oldCurrentWindow, m_currentWindow);
}

/// \brief Retrieves the about text for the application.
/// \return A constant reference to a QString containing the about text.
const QString& aboutText()
{
    static const QString text = QString(
               "<b>%1</b><br>"
               "Version: %2<br>"
               "<p>This is a free software.</p>"
               "<hr>"
               "<p>Credits:</p>"
               "<br> - QT framework: <a href=\"http://qt.io\">Site</a>"
               "<br> - Qt Advanced Docking System: <a "
               "href=\"https://github.com/githubuser0xFFFF/"
               "Qt-Advanced-Docking-System\">Site</a>"
               "<br> - qtpropertybrowser: <a "
               "href=\"https://github.com/qtproject/qt-solutions/tree/master/"
               "qtpropertybrowser\">Site</a>"
    )
        .arg(QApplication::applicationDisplayName(), VERSION);

    return text;
}