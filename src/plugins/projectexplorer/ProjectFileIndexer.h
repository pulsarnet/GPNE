// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef NETFILEINDEX_H
#define NETFILEINDEX_H

#include <corelib/FileRef.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/callable_traits/return_type.hpp>

namespace bmi = boost::multi_index;

class Project;
class IDocument;

struct FileIndexEntry {
    FileRef file;
    QString directory;
    QString subdir;

    FileIndexEntry(const FileRef& f, const QString& d, const QString& s)
        : file(f), directory(d), subdir(s) {}
};

QString filenameLower(const FileIndexEntry& entry);

class ProjectFileIndexer : public QObject {

    Q_OBJECT

public:

    static ProjectFileIndexer* instance(Project*);
    static void addRootSubdir(QString subdir);
    static QList<FileRef> files(Project* project, const QString& subdir);

public slots:

    void onDirectoryChanged(const QString& path);
    void onDocumentAdded(IDocument* document);
    void onDocumentRemoved(IDocument* document);
    void onDocumentNameChanged(FileRef old, FileRef now);

signals:

    void indexChanged(QString subdir);

private:

    explicit ProjectFileIndexer(Project*);
    void scanDirectory(const QString& subdir);
    QString getSubdirFromPath(const QString& path) const;

    struct by_file {};
    struct by_directory {};
    struct by_subdir {};
    struct by_filename {};

    using FileIndex = boost::multi_index_container<
        FileIndexEntry,
        bmi::indexed_by<
            bmi::ordered_non_unique<
                bmi::tag<by_filename>,
                bmi::global_fun<const FileIndexEntry&, boost::callable_traits::return_type_t<decltype(&filenameLower)>, &filenameLower>
            >,
            bmi::hashed_unique<
                bmi::tag<by_file>,
                bmi::member<FileIndexEntry, decltype(FileIndexEntry::file), &FileIndexEntry::file>
            >,
            bmi::hashed_non_unique<
                bmi::tag<by_directory>,
                bmi::member<FileIndexEntry, decltype(FileIndexEntry::directory), &FileIndexEntry::directory>
            >,
            bmi::hashed_non_unique<
                bmi::tag<by_subdir>,
                bmi::member<FileIndexEntry, decltype(FileIndexEntry::subdir), &FileIndexEntry::subdir>
            >

        >
    >;

    QString m_rootDirectory;
    FileIndex m_fileIndex;
};

#endif //NETFILEINDEX_H
