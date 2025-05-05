// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROJECTFILEMANAGER_H
#define PROJECTFILEMANAGER_H

#include <QObject>

class QFileSystemWatcher;
class Project;

class FileRef {
public:
    explicit FileRef();
    explicit FileRef(const QString& path);

    QString absolutePath() const;
    QString filename() const;
    QString extension() const;
    QString dir() const;

    bool isEmpty() const;

    bool operator==(const FileRef& other) const;

private:

    QString m_basename;
    QString m_extension;
    QString m_dir;

};

Q_DECLARE_METATYPE(FileRef)

uint qHash(const FileRef &file, uint seed);

namespace boost {
    inline std::size_t hash_value(const QString& str) {
        return std::hash<QString>()(str);
    }

    inline std::size_t hash_value(const FileRef& file) {
        return std::hash<QString>()(file.absolutePath());
    }
}

#endif //PROJECTFILEMANAGER_H
