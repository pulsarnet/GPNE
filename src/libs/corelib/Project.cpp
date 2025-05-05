// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Project.h"
#include <rfl/xml.hpp>
#include "ProjectUtil.h"
#include "ReflectionExt.h"

std::expected<ProjectData, QString> persistence::readProjectData(const std::filesystem::path &path) {
    const auto result = rfl::xml::load<ProjectData>(path.string());
    if (!result) {
        return std::unexpected(QString::fromStdString("Failed to load project data: " + result.error().what()));
    }
    return std::move(result.value());
}

std::expected<void, QString> persistence::writeProjectData(const ProjectData &data, const std::filesystem::path &path) {
    const auto str = rfl::xml::write<"Project">(data);
    std::ofstream output(path.string());
    if (!output) {
        return std::unexpected(QString::fromStdString("Failed to open file: " + path.string()));
    }

    output.write(str.data(), str.size());
    if (!output) {
        return std::unexpected(QString::fromStdString("Failed to write file: " + path.string()));
    }

    return {};
}

std::expected<Project::Ptr, QString> Project::load(const std::filesystem::path &path) {
    const auto data = persistence::readProjectData(path);
    if (!data) {
        return std::unexpected(data.error());
    }

    const auto pathStr = ProjectUtil::canonicalPath(QString::fromStdString(path.string()));
    const auto filePath = ProjectUtil::getProjectFilePath(pathStr);
    const auto root = ProjectUtil::getParentDirectory(filePath);

    return std::unique_ptr<Project>(new Project(data.value(), root.toStdString(), filePath.toStdString()));
}
