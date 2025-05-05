// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Settings.h"

#include <qcoreapplication.h>
#include <QDir>
#include <QSettings>

void Settings::setLastOpenedProjectDirectory(const QString &path) {
    QSettings settings;
    settings.setValue("FS/LastOpenedDirectory", path);
}

QString Settings::lastOpenedProjectDirectory() {
    QSettings settings;
    static QString path = QDir::homePath() + "/" + QCoreApplication::applicationName() + "Projects";
    return settings.value("FS/LastOpenedDirectory", path).toString();
}

void Settings::setLastParentPath(const QString &path) {
    QSettings settings;
    settings.setValue("FS/LastParentPath", path);
}

QString Settings::lastParentPath() {
    QSettings settings;
    static QString path = QDir::homePath() + "/" + QCoreApplication::applicationName() + "Projects";
    return settings.value("FS/LastParentPath", QDir::homePath()).toString();
}
