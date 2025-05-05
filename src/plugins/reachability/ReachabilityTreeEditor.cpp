// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ReachabilityTreeEditor.h"
#include "ReachabilityDocument.h"
#include "ReachabilityWindow.h"
#include <corelib/Constants.h>
#include <corelib/EditorManager.h>
#include "Constants.h"

ReachabilityTreeEditor::ReachabilityTreeEditor(IDocument *document) : IEditor(document) {

}

QString ReachabilityTreeEditor::displayName() const {
    return tr("Reachability Tree");
}

QIcon ReachabilityTreeEditor::icon() const {
    return QIcon(Constants::TREE_ICON);
}

QWidget* ReachabilityTreeEditor::editorWidget() {
    auto reachabilityDocument = qobject_cast<ReachabilityDocument*>(document());
    auto reachabilityWindow = new ReachabilityWindow(reachabilityDocument);
    return reachabilityWindow;
}

IEditor* ReachabilityTreeEditorFactory::createEditor(IDocument *document) {
    if (document->documentType() != Constants::REACHABILITY_DOC_TYPE) {
        return nullptr;
    }
    return new ReachabilityTreeEditor(document);
}

bool ReachabilityTreeEditorFactory::isDocumentTypeSupported(const QString &documentType) const {
    return documentType == Constants::REACHABILITY_DOC_TYPE;
}


