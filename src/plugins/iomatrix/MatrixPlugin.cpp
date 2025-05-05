// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "MatrixPlugin.h"
#include "MatrixWindow.h"
#include <corelib/ActionManager.h>
#include <corelib/ApplicationProjectController.h>
#include <corelib/Constants.h>
#include <corelib/EditorManager.h>
#include <corelib/MainWindow.h>
#include <corelib/IEditor.h>
#include <editor/PetriNetDocument.h>
#include <editor/Constants.h>
#include "Constants.h"

void MatrixPlugin::initialize() {
    QAction* matrixWindow = new QAction(QIcon(Constants::GRID_ICON), tr("Matrix"), nullptr);
    matrixWindow->setEnabled(false);
    connect(matrixWindow, &QAction::triggered, this, &MatrixPlugin::onMatrixWindowRequested);

    ActionManager::registerAction(Constants::A_MATRIX, matrixWindow);
    ActionManager::group(Constants::TOOLS_GROUP)->addAction(Constants::A_MATRIX);

    connect(ApplicationProjectController::instance(), &ApplicationProjectController::editorOpened, this, &MatrixPlugin::onEditorOpened);
}

void MatrixPlugin::onEditorOpened(IEditor *editor) {
    // attach action
    if (!editor || editor->document()->documentType() != Constants::PETRI_NET_DOC_TYPE) {
        return;
    }

    auto action = ActionManager::action(Constants::A_MATRIX);
    if (action) {
        qDebug() << "Attach action" << action << editor;
        ActionManager::attachAction(action, editor->widget());
    }
}

void MatrixPlugin::onMatrixWindowRequested() {
    auto currentEditor = EditorManager::currentEditor();
    auto document = currentEditor->document();
    if (auto net = qobject_cast<PetriNetDocument*>(document)) {
        auto matrix = new MatrixWindow(net->model(), currentEditor->widget());
        matrix->show();
    }
}
