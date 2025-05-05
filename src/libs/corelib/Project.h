// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_PROJECTMETADATA_H
#define GPNE_PROJECTMETADATA_H

#include <expected>
#include <filesystem>
#include <QDateTime>
#include <QObject>
#include <rfl/Attribute.hpp>
#include <rfl/Timestamp.hpp>

struct ProjectData {
    QString id;
    QString name;
    QDateTime created;
    std::optional<QString> author;
};

namespace persistence {
    std::expected<ProjectData, QString> readProjectData(const std::filesystem::path&);
    std::expected<void, QString> writeProjectData(const ProjectData& data, const std::filesystem::path&);
}

class Project final : public QObject {

    Q_OBJECT

    explicit Project(ProjectData data, const std::filesystem::path& root, const std::filesystem::path& filePath)
        : m_data(std::move(data)), m_root(root), m_filePath(filePath) {}

public:

    Project() = delete;
    Q_DISABLE_COPY_MOVE(Project);

    using Ptr = std::unique_ptr<Project>;

    static std::expected<Ptr, QString> load(const std::filesystem::path& path);

    const QString& name() const { return m_data.name; }
    const std::filesystem::path& root() const { return m_root; }
    const std::filesystem::path& filePath() const { return m_filePath; }

private:

    ProjectData m_data;
    std::filesystem::path m_root;
    std::filesystem::path m_filePath;
};
#endif // GPNE_PROJECTMETADATA_H
