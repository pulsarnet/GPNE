// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_RECENTPROJECTS_H
#define GPNE_RECENTPROJECTS_H

#include <QObject>

struct RecentProject {
    QString name;
    QString path;
};

struct RecentProjectsImpl {
    std::vector<RecentProject> projects;
};

class RecentProjectsManager : public QObject {
    Q_OBJECT
public:
    static RecentProjectsManager* instance();

    const std::vector<RecentProject>& getProjects() { return m_projects; }

    void addProject(const QString& name, const QString& path);
    void removeProject(const QString& path);

signals:
    void projectsChanged();

private:
    RecentProjectsManager();

    void readProjects();
    void writeProjects();

    std::vector<RecentProject> m_projects;
};

#endif // GPNE_RECENTPROJECTS_H
