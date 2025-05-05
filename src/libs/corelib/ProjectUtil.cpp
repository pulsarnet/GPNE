// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ProjectUtil.h"
#include <QDir>

#define GPNE_PROJECT_EXTENSION "gpneproj"
#define GPNE_PROJECT_BASE_NAME "project"
#define GPNE_PROJECT_FILE_NAME "project.gpneproj"
#define GPNE_NET_FILE_NAME "net.json"

QString ProjectUtil::getNetFilePath(const QString &root) {
    QDir dir(canonicalPath(root));
    return dir.absoluteFilePath(GPNE_NET_FILE_NAME);
}

bool ProjectUtil::isProjectFile(const QString &path) {
    return QFileInfo(canonicalPath(path)).suffix() == GPNE_PROJECT_EXTENSION;
}

/**
 * \brief Checks if the given directory is an existing project directory.
 *
 * This function verifies if the specified root directory exists and contains
 * the project file defined by GPNE_PROJECT_FILE_NAME.
 *
 * \param root The root directory to check.
 * \return true if the directory exists and contains the project file, false otherwise.
 */
bool ProjectUtil::isExistingProjectDirectory(const QString& root){
    QDir dir(canonicalPath(root));
    return dir.exists() && dir.exists(GPNE_PROJECT_FILE_NAME);
}

QString ProjectUtil::getProjectFilePath(const QString &root) {
    const auto canonicalPath = ProjectUtil::canonicalPath(root);
    if (isProjectFile(canonicalPath)) {
        return canonicalPath;
    }

    QDir dir(canonicalPath);
    return dir.absoluteFilePath(GPNE_PROJECT_FILE_NAME);
}

QString ProjectUtil::canonicalPath(const QString &path) {
    QFileInfo info(path);
    return info.isAbsolute() ? info.absoluteFilePath() : QString();
}

QString ProjectUtil::getParentDirectory(const QString &path) {
    return QFileInfo(canonicalPath(path)).dir().absolutePath();
}
