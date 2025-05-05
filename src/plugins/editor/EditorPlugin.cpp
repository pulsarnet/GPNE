// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "EditorPlugin.h"
#include <QMenu>
#include <corelib/ActionManager.h>
#include <corelib/EditorManager.h>
#include "PetriNetEditor.h"
#include <corelib/MainWindow.h>
#include <QRegularExpression>
#include <corelib/ApplicationProjectController.h>

#include "Constants.h"
#include "EditorToolController.h"
#include "EditorWidget.h"

void EditorPlugin::initialize() {
    EditorManager::registerEditorFactory(Constants::PETRI_NET_EDITOR, new PetriNetEditorFactory());

    connect(ApplicationProjectController::instance(), &ApplicationProjectController::editorOpened, this, &EditorPlugin::onEditorOpened);
}

void EditorPlugin::onEditorOpened(IEditor *editor) {
    auto petriNetEditor = qobject_cast<PetriNetEditor*>(editor);
    if (!petriNetEditor) {
        return;
    }

    auto editorWidget = qobject_cast<EditorWidget*>(petriNetEditor->widget());
    if (!editorWidget) {
        return;
    }

    new EditorToolController(editorWidget->view(), petriNetEditor);
}
