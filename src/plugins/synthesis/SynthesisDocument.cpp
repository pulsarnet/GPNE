// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SynthesisDocument.h"

#include "Constants.h"

SynthesisDocument::SynthesisDocument(shared_decomposition_t decomposition) : IDocument(), m_decomposition(decomposition) {

}

QString SynthesisDocument::displayName() const {
    return tr("Synthesis");
}

bool SynthesisDocument::save(const FileRef &targetPath, QString &errorString) {
    Q_UNUSED(targetPath);
    Q_UNUSED(errorString);
    return true;
}

bool SynthesisDocument::reload(const FileRef &sourcePath, QString &errorString) {
    Q_UNUSED(sourcePath);
    Q_UNUSED(errorString);
    return true;
}

QString SynthesisDocument::documentType() const {
    return Constants::SYNTHESIS_DOC_TYPE;
}

SynthesisDocument::shared_decomposition_t SynthesisDocument::decomposition() {
    return m_decomposition;
}
