// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SimulationPlugin.h"

#include <QWidget>
#include <corelib/ApplicationProjectController.h>
#include <editor/EditorWidget.h>
#include <editor/PetriNetEditor.h>

#include "SimulationWidget.h"

void SimulationPlugin::initialize()
{
    qDebug() << "Initialize Simulation plugin";
    connect(ApplicationProjectController::instance(), &ApplicationProjectController::editorOpened, this, &SimulationPlugin::onEditorOpened);
}

void SimulationPlugin::onEditorOpened(IEditor* editor)
{
    auto petriNetEditor = qobject_cast<PetriNetEditor*>(editor);
    if (!petriNetEditor) {
        return;
    }

    auto editorWidget = qobject_cast<EditorWidget*>(petriNetEditor->widget());
    if (!editorWidget) {
        return;
    }

    new SimulationWidget(editorWidget->view());
}