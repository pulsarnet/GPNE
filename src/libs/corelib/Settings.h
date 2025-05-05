// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings {
public:
    static void setLastOpenedProjectDirectory(const QString& path);
    static QString lastOpenedProjectDirectory();

    static void setLastParentPath(const QString& path);
    static QString lastParentPath();
};

#endif //SETTINGS_H
