// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ProjectFileIndexer.h"

#include <corelib/DocumentManager.h>
#include <corelib/IDocument.h>
#include <QDir>
#include <QFileSystemWatcher>
#include <corelib/Project.h>
#include <QHash>

QHash<Project*, ProjectFileIndexer*> instances;
QList<QString> rootSubdirs;

ProjectFileIndexer::ProjectFileIndexer(Project* project) : QObject(project)
{
    m_rootDirectory = QString::fromStdString(project->root().string());
    if (!m_rootDirectory.endsWith("/")) {
        m_rootDirectory += "/";
    }

    for (const QString& subdir : rootSubdirs) {
        const QString path = m_rootDirectory + subdir;
        QDir dir(path);
        if (dir.exists()) {
            auto watcher = new QFileSystemWatcher(this);
            watcher->addPath(path);
            connect(watcher, &QFileSystemWatcher::directoryChanged, this, &ProjectFileIndexer::onDirectoryChanged);
        }
        scanDirectory(subdir);
    }

    connect(DocumentManager::instance(), &DocumentManager::documentAdded, this, &ProjectFileIndexer::onDocumentAdded);
    connect(DocumentManager::instance(), &DocumentManager::documentRemoved, this, &ProjectFileIndexer::onDocumentRemoved);
}

void ProjectFileIndexer::scanDirectory(const QString& subdir)
{
    QDir dir(m_rootDirectory + subdir);
    qDebug() << "Scan dir: " << dir.absolutePath();
    if (dir.exists()) {
        const QFileInfoList f = dir.entryInfoList(
            QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase
        );
        for (const QFileInfo &fi : f) {
            qDebug() << "Add file: " << fi.absoluteFilePath();
            FileRef fileRef(QDir::cleanPath(fi.absoluteFilePath()));
            m_fileIndex.insert(FileIndexEntry(fileRef, dir.absolutePath(), subdir));
        }
    }
    emit indexChanged(subdir);
}

QString filenameLower(const FileIndexEntry &entry) {
    return entry.file.filename().toLower();
}

ProjectFileIndexer* ProjectFileIndexer::instance(Project* project)
{
    auto instance = instances.value(project, nullptr);
    if (!instance) {
        instance = new ProjectFileIndexer(project);
        instances.insert(project, instance);
    }
    return instance;
}

/*
 * Adds a root subdirectory to the list of directories to watch for changes.
 *
 * \param subdir The subdirectory relative to the project root.
 */
void ProjectFileIndexer::addRootSubdir(QString subdir)
{
    QDir path(subdir);
    if (!path.isRelative()) {
        return;
    }
    if (subdir.endsWith("/")) {
        subdir.chop(1);
    }

    rootSubdirs.removeAll(subdir);
    rootSubdirs.append(subdir);
}

QList<FileRef> ProjectFileIndexer::files(Project* project, const QString& subdir)
{
    if (!rootSubdirs.contains(subdir)) {
        return {};
    }
    auto instance = ProjectFileIndexer::instance(project);

    QList<FileRef> files;
    const auto& bySubdir = instance->m_fileIndex.get<by_subdir>();
    auto range = bySubdir.equal_range(subdir);
    for (auto it = range.first; it != range.second; ++it) {
        files.append(it->file);
    }
    return files;
}

void ProjectFileIndexer::onDirectoryChanged(const QString& path)
{
    QSet<FileRef> oldFiles;
    QString subdir = getSubdirFromPath(path);

    // Get old files
    auto& byDir = m_fileIndex.get<by_directory>();
    auto range = byDir.equal_range(path);
    for (auto it = range.first; it != range.second; ++it) {
        oldFiles.insert(it->file);
    }

    // Get new files
    QSet<FileRef> newFiles;
    QDir dir(path);
    if (dir.exists()) {
        const QFileInfoList f = dir.entryInfoList(
            QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase
        );
        for (const QFileInfo &fi : f) {
            newFiles.insert(FileRef(QDir::cleanPath(fi.absoluteFilePath())));
        }
    }

    // Update index
    QSet<FileRef> addedFiles = newFiles - oldFiles;
    QSet<FileRef> removedFiles = oldFiles - newFiles;

    // Remove old files
    auto& byFile = m_fileIndex.get<by_file>();
    for (const auto& file : removedFiles) {
        byFile.erase(file);
    }

    // Add new files
    for (const auto& file : addedFiles) {
        m_fileIndex.insert(FileIndexEntry(file, path, subdir));
    }

    emit indexChanged(subdir);
}

QString ProjectFileIndexer::getSubdirFromPath(const QString& path) const {
    return path.startsWith(m_rootDirectory)
        ? path.mid(m_rootDirectory.length())
        : QString();
}

void ProjectFileIndexer::onDocumentAdded(IDocument* document)
{
    // each document connect to ProjectFileIndexer because it can change filename and indexing under directory
    connect(document, &IDocument::filenameChanged, this, &ProjectFileIndexer::onDocumentNameChanged);

    if (document->file().isEmpty()) {
        return;
    }

    QFileInfo info(document->file().absolutePath());
    QString path = info.absolutePath();
    if (!path.isEmpty() && path.contains(m_rootDirectory)) {
        QString subdir = getSubdirFromPath(path);
        m_fileIndex.insert(FileIndexEntry(document->file(), path, subdir));

        emit indexChanged(subdir);
    }
}

void ProjectFileIndexer::onDocumentRemoved(IDocument* document)
{
    disconnect(document, &IDocument::filenameChanged, this, &ProjectFileIndexer::onDocumentNameChanged);
    if (document->file().isEmpty()) {
        return;
    }

    QFileInfo info(document->file().absolutePath());
    QString path = info.absolutePath();
    QString subdir = getSubdirFromPath(path);

    auto& byFile = m_fileIndex.get<by_file>();
    auto it = byFile.find(document->file());
    if (it != byFile.end()) {
        byFile.erase(it);
        emit indexChanged(subdir);
    }
}

void ProjectFileIndexer::onDocumentNameChanged(FileRef old, FileRef now)
{
    QFileInfo oldInfo(old.absolutePath());
    QFileInfo newInfo(now.absolutePath());

    QString oldPath = oldInfo.absolutePath();
    QString newPath = newInfo.absolutePath();
    QString oldSubdir = getSubdirFromPath(oldPath);
    QString newSubdir = getSubdirFromPath(newPath);

    auto& byFile = m_fileIndex.get<by_file>();
    auto it = byFile.find(old);

    if (it != byFile.end()) {
        byFile.erase(it);

        m_fileIndex.insert(FileIndexEntry(now, newPath, newSubdir));

        if (oldSubdir != newSubdir) {
            emit indexChanged(oldSubdir);
            emit indexChanged(newSubdir);
        } else {
            emit indexChanged(oldSubdir);
        }
    }
}

