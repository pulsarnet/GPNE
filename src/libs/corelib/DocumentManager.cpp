// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "DocumentManager.h"
#include <QWidget>
#include "IDocument.h"
#include "FileRef.h"

/// \brief Retrieves the singleton instance of DocumentManager.
/// \return A pointer to the singleton instance of DocumentManager.
DocumentManager* DocumentManager::instance() {
    static DocumentManager instance;
    return &instance;
}

/// \brief Adds a document to the DocumentManager.
/// \param document The document to be added.
void DocumentManager::addDocument(IDocument *document) {
    if (!document) {
        return;
    }

    auto instance = DocumentManager::instance();
    auto file = document->file();
    if (!file.isEmpty()) {
        instance->m_documentsMap.insert(document->file(), document);
    }

    connect(document, &IDocument::filenameChanged, instance, &DocumentManager::onDocumentFilenameChanged);
    connect(document, &IDocument::destroyed, instance, [document] {
        DocumentManager::removeDocument(document);
    });
    emit instance->documentAdded(document);
}

/// \brief Removes a document from the DocumentManager.
/// \param document The document to be removed.
void DocumentManager::removeDocument(IDocument *document) {
    qDebug() << "DocumentManager::removeDocument";
    auto instance = DocumentManager::instance();
    instance->m_documents.remove(document);
    instance->m_documentsMap.remove(document->file());

    disconnect(document, &IDocument::filenameChanged, instance, &DocumentManager::onDocumentFilenameChanged);
    disconnect(document, &IDocument::destroyed, instance, nullptr);
    emit instance->documentRemoved(document);
}

/// \brief Saves a document.
/// \param document The document to be saved.
void DocumentManager::saveDocument(IDocument* document) {
    Q_UNUSED(document);
}

/// \brief Retrieves a document by its file reference.
/// \param file The file reference of the document.
/// \return A pointer to the document, or nullptr if not found.
IDocument* DocumentManager::getDocument(const FileRef& file) {
    if (file.isEmpty()) {
        return nullptr;
    }
    return instance()->m_documentsMap.value(file, nullptr);
}

/// \brief Handles the event when a document's filename is changed.
/// \param old The old file reference.
/// \param now The new file reference.
void DocumentManager::onDocumentFilenameChanged(FileRef old, FileRef now) {
    // move document
    auto document = qobject_cast<IDocument*>(sender());
    if (!document) {
        return;
    }

    m_documentsMap.remove(old);
    if (!now.isEmpty()) {
        m_documentsMap.insert(now, document);
    }
}
