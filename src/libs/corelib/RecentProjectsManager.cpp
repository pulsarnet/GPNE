// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "RecentProjectsManager.h"
#include "Project.h"
#include "ProjectUtil.h"
#include <QDir>
#include <QJsonDocument>
#include <QSettings>

RecentProjectsManager* RecentProjectsManager::instance() {
    static RecentProjectsManager* instance = new RecentProjectsManager();
    return instance;
}

RecentProjectsManager::RecentProjectsManager() {
    readProjects();
}

void RecentProjectsManager::addProject(const QString& name, const QString& path)
{
    Q_ASSERT(!path.isEmpty());

    auto canonicalPath = ProjectUtil::canonicalPath(path);
    auto it = std::ranges::find_if(m_projects, [&canonicalPath](auto& proj) { return proj.path == canonicalPath; });
    if (it != m_projects.end()) {
        std::iter_swap(it, m_projects.begin());
        emit projectsChanged();
    } else {
        RecentProject recent{
            .name = name,
            .path = canonicalPath,
        };
        m_projects.insert(m_projects.begin(), recent);
        emit projectsChanged();
    }
    writeProjects();
}

void RecentProjectsManager::removeProject(const QString& path)
{
    Q_ASSERT(!path.isEmpty());

    auto canonicalPath = ProjectUtil::canonicalPath(path);
    auto toRemove = std::ranges::find_if(m_projects, [&canonicalPath](auto& proj) { return proj.path == canonicalPath; });
    m_projects.erase(toRemove, m_projects.end());

    writeProjects();
}

void RecentProjectsManager::readProjects()
{
    QSettings settings;
    int size = settings.beginReadArray("Projects");

    m_projects.clear();

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QVariant pathVariant = settings.value("path");
        QVariant openedVariant = settings.value("opened");
        if (pathVariant.isNull() || pathVariant.typeId() != QMetaType::Type::QString) {
            continue;
        }

        QString path = pathVariant.toString();
        const auto result = persistence::readProjectData(path.toStdString());
        if (result.has_value()) {
            RecentProject recent{
                .name = result.value().name,
                .path = path,
            };
            m_projects.insert(m_projects.begin(), recent);
        }
    }
}

void RecentProjectsManager::writeProjects()
{
    QSettings settings;
    settings.beginWriteArray("Projects");

    for (int i = 0; i < m_projects.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("name", m_projects[i].name);
        settings.setValue("path", m_projects[i].path);
    }
    settings.endArray();
    settings.sync();
}
