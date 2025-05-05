// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "FileRef.h"
#include <QDir>
#include <QFileInfo>

FileRef::FileRef()
{
}

/// \brief Constructs a FileRef object from a given file path.
/// \param path The file path to initialize the FileRef object.
FileRef::FileRef(const QString& path)
{
    QFileInfo info(path);
    if (info.isAbsolute()) {
        m_basename = info.baseName();
        m_extension = info.suffix();
        m_dir = info.absolutePath();
    }
}

/// \brief Retrieves the absolute path of the file.
/// \return A QString containing the absolute path of the file.
QString FileRef::absolutePath() const
{
    return QDir::cleanPath(m_dir + "/" + m_basename + "." + m_extension);
}

/// \brief Retrieves the base name of the file.
/// \return A QString containing the base name of the file.
QString FileRef::filename() const {
    return m_basename;
}

/// \brief Retrieves the extension of the file.
/// \return A QString containing the extension of the file.
QString FileRef::extension() const { return m_extension; }

QString FileRef::dir() const { return m_dir; }

/// \brief Checks if the FileRef object is empty.
/// \return True if the FileRef object is empty, false otherwise.
bool FileRef::isEmpty() const {
    return m_basename.isEmpty();
}

bool FileRef::operator==(const FileRef& other) const
{
    return m_dir == other.m_dir && m_basename == other.m_basename && m_extension == other.m_extension;
}

uint qHash(const FileRef &file, uint seed) {
    return qHash(file.absolutePath(), seed);
}
