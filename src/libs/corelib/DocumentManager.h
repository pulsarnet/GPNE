// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include "FileRef.h"
#include <QSet>

class IDocument;

// NOTE: The DocumentManager class only handles documents that are currently open in the application.
// It does not manage documents on a per-project basis.
// To manage documents per project, link the document to the project using your own structure.

class DocumentManager : public QObject {

    Q_OBJECT

public:

    static DocumentManager* instance();

    static void addDocument(IDocument* document);
    static void removeDocument(IDocument* document);
    static void saveDocument(IDocument* document);

    static IDocument* getDocument(const FileRef& file);

signals:

    void documentAdded(IDocument* document);
    void documentRemoved(IDocument* document);

private slots:

    void onDocumentFilenameChanged(FileRef old, FileRef now);

private:

    DocumentManager() = default;

    QSet<IDocument*> m_documents;
    QHash<FileRef, IDocument*> m_documentsMap;

};

#endif //DOCUMENTMANAGER_H
