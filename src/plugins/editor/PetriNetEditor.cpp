// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "PetriNetEditor.h"
#include "PetriNetDocument.h"
#include "EditorWidget.h"
#include <corelib/ActionManager.h>
#include "Constants.h"
#include <corelib/IDocument.h>

PetriNetEditor::PetriNetEditor(IDocument *document): IEditor(document) {
}

QString PetriNetEditor::displayName() const {
    return document()->displayName();
}

QIcon PetriNetEditor::icon() const {
    return QIcon(":/images/icons/graph.svg");
}

QWidget* PetriNetEditor::editorWidget() {
    auto realDocument = qobject_cast<PetriNetDocument*>(document());
    auto editor = new EditorWidget(realDocument->model(), nullptr);
    return editor;
}

IEditor* PetriNetEditorFactory::createEditor(IDocument *document) {
    return new PetriNetEditor(document);
}

bool PetriNetEditorFactory::isDocumentTypeSupported(const QString &documentType) const {
    return documentType == Constants::PETRI_NET_DOC_TYPE;
}
