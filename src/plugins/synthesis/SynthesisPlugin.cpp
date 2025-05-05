// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SynthesisPlugin.h"

#include "Constants.h"
#include <corelib/ActionManager.h>
#include <corelib/ApplicationProjectController.h>
#include <corelib/Constants.h>
#include <corelib/DocumentManager.h>
#include <corelib/EditorManager.h>
#include <corelib/Project.h>
#include <corelib/IDocument.h>
#include <corelib/IEditor.h>
#include <editor/Constants.h>
#include <editor/model/NetModel.h>
#include <editor/PetriNetDocument.h>
#include <ptn/net.h>
#include <QAction>
#include <QPointer>

#include "DecomposeTask.h"
#include "SynthesisController.h"
#include "SynthesisDocument.h"
#include "SynthesisEditor.h"
#include "SynthesisEditorWidget.h"

void SynthesisPlugin::initialize() {
    m_synthesisAction = new QAction(QIcon(Constants::SYNTH_ICON), tr("Synthesis"), this);
    m_synthesisAction->setEnabled(false);
    connect(m_synthesisAction, &QAction::triggered, this, &SynthesisPlugin::onSynthesisAction);

    m_runDirectedSynthesisAction = new QAction(QIcon(Constants::SYNTH_ICON), tr("Run Directed Synthesis"), this);
    m_runDirectedSynthesisAction->setEnabled(false);
    connect(m_runDirectedSynthesisAction, &QAction::triggered, this, &SynthesisPlugin::onRunDirectedSynthesisAction);

    ActionManager::registerAction(Constants::A_SYNTHESIS, m_synthesisAction);
    ActionManager::registerAction(Constants::A_RUN_DIRECTED_SYNTHESIS, m_runDirectedSynthesisAction);

    ActionManager::group(Constants::TOOLS_GROUP)->addAction(Constants::A_SYNTHESIS);
    ActionManager::group(Constants::TOOLS_GROUP)->addAction(Constants::A_RUN_DIRECTED_SYNTHESIS);

    EditorManager::registerEditorFactory(Constants::SYNTHESIS_EDITOR, new SynthesisEditorFactory());

    connect(ApplicationProjectController::instance(), &ApplicationProjectController::editorOpened, this, &SynthesisPlugin::onEditorOpened);
}

void SynthesisPlugin::onSynthesisAction() {
    QPointer project = ApplicationProjectController::currentProject();
    if (auto petriNetDocument = qobject_cast<PetriNetDocument*>(EditorManager::currentEditor()->document())) {
        auto document = m_synthesisDocuments.value(petriNetDocument, nullptr);
        if (!document) {
            auto task = new DecomposeTask(petriNetDocument->model()->net());
            connect(task, &DecomposeTask::decompositionReady, this, [this, petriNetDocument, project](DecomposeTask::shared_return_t decomposition) {
                auto synthesisDocument = new SynthesisDocument(decomposition);
                DocumentManager::addDocument(synthesisDocument);
                m_synthesisDocuments.insert(petriNetDocument, synthesisDocument);
                EditorManager::openEditor(project, synthesisDocument);
            });
            m_lockedDocuments.insert(petriNetDocument);
            connect(task, &DecomposeTask::destroyed, this, [this, petriNetDocument]() {
                m_lockedDocuments.remove(petriNetDocument);
            });
            ApplicationProjectController::runBackgroundTask(project, task);
        } else {
            EditorManager::openEditor(project, document);
        }
    }
}

void SynthesisPlugin::onRunDirectedSynthesisAction() {
    qDebug() << "Run directed synthesis";
    if (auto synthesisEditor = qobject_cast<SynthesisEditor*>(EditorManager::currentEditor())) {
        if (auto widget = qobject_cast<SynthesisEditorWidget*>(synthesisEditor->widget())) {
            widget->controller()->onStartRequested();
        }
    }
}

void SynthesisPlugin::onEditorOpened(IEditor* editor) {
    if (editor->document()) {
        if (editor->document()->documentType() == Constants::PETRI_NET_DOC_TYPE) {
            auto action = ActionManager::action(Constants::A_SYNTHESIS);
            if (action) {
                ActionManager::attachAction(action, editor->widget());
            }
        } else if (editor->document()->documentType() == Constants::SYNTHESIS_DOC_TYPE) {
            auto action = ActionManager::action(Constants::A_RUN_DIRECTED_SYNTHESIS);
            if (action) {
                ActionManager::attachAction(action, editor->widget());
            }
        }
    }
}
