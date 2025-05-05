// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef IDOCUMENT_H
#define IDOCUMENT_H

#include "FileRef.h"

class IDocument : public QObject {

    Q_OBJECT

public:

    virtual QString displayName() const = 0;

    const FileRef& file() const;
    void setFile(const FileRef& file);

    virtual bool save(const FileRef& targetPath, QString& errorString) = 0;
    virtual bool reload(const FileRef& sourcePath, QString& errorString) = 0;

    virtual QString fileExtension() const;
    virtual QString documentType() const = 0;

    bool isModified() const noexcept;
    void setModified(bool modified);

    virtual bool isInternalDocument() const noexcept;

signals:

    void contentChanged();
    void modificationChanged(bool modified);
    void filenameChanged(FileRef old, FileRef now);
    void documentAboutToSave();

private:

    FileRef m_file;
    bool m_modified = false;

};

#endif //IDOCUMENT_H
