// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROJECTUTIL_H
#define PROJECTUTIL_H

#include <QString>

class ProjectUtil {
public:
    static QString getNetFilePath(const QString& root);

    static bool isProjectFile(const QString& path);
    static bool isExistingProjectDirectory(const QString& root);
    static QString getProjectFilePath(const QString& root);
    static QString canonicalPath(const QString& path);
    static QString getParentDirectory(const QString& path);
};

#endif //PROJECTUTIL_H
