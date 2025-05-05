// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SynthesisEditor.h"

#include "Constants.h"
#include "SynthesisEditorWidget.h"

SynthesisEditor::SynthesisEditor(IDocument* document) : IEditor(document) {}

QString SynthesisEditor::displayName() const {
    return tr("Synthesis");
}

QIcon SynthesisEditor::icon() const {
    return QIcon(Constants::SYNTH_ICON);
}

QWidget* SynthesisEditor::editorWidget() {
    return new SynthesisEditorWidget(qobject_cast<SynthesisDocument*>(document()));
}

IEditor* SynthesisEditorFactory::createEditor(IDocument *document) {
    return new SynthesisEditor(document);
}

bool SynthesisEditorFactory::isDocumentTypeSupported(const QString &documentType) const {
    return documentType == Constants::SYNTHESIS_DOC_TYPE;
}


