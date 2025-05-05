// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "IDocument.h"
#include "FileRef.h"

const FileRef& IDocument::file() const {
    return m_file;
}

void IDocument::setFile(const FileRef &file) {
    if (m_file == file) {
        return;
    }

    FileRef old(m_file);
    m_file = file;
    emit filenameChanged(old, m_file);
}

QString IDocument::fileExtension() const {
    return "";
}

bool IDocument::isModified() const noexcept {
    return m_modified;
}

void IDocument::setModified(bool modified) {
    if (m_modified == modified) {
        return;
    }
    m_modified = modified;
    emit modificationChanged(modified);
}

bool IDocument::isInternalDocument() const noexcept {
    return false;
}
