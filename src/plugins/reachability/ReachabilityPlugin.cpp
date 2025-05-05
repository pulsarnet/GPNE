// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ReachabilityPlugin.h"

#include <corelib/ActionManager.h>
#include <corelib/ApplicationProjectController.h>
#include <corelib/Constants.h>
#include <corelib/DocumentManager.h>
#include <corelib/EditorManager.h>
#include <corelib/Project.h>
#include <editor/Constants.h>
#include <editor/model/NetModel.h>
#include <editor/PetriNetDocument.h>
#include <QPointer>

#include "BuildReachabilityGraphTask.h"
#include "Constants.h"
#include "ReachabilityDocument.h"
#include "ReachabilityTreeEditor.h"
#include "ReachabilityWindow.h"

void ReachabilityPlugin::initialize() {
    m_reachabilityAction = new QAction(QIcon(Constants::TREE_ICON), tr("Reachability Tree"), this);
    m_reachabilityAction->setEnabled(false);
    connect(m_reachabilityAction, &QAction::triggered, this, &ReachabilityPlugin::onReachabilityTreeRequested);

    m_reloadAction = new QAction(QIcon(Constants::ICON_RELOAD), tr("Reload"), this);
    connect(m_reloadAction, &QAction::triggered, this, &ReachabilityPlugin::onReloadRequested);

    m_fitInViewAction = new QAction(QIcon(Constants::FIT_IN_VIEW_ICON), tr("Fit In View"), this);
    connect(m_fitInViewAction, &QAction::triggered, this, &ReachabilityPlugin::onFitInView);


    ActionManager::registerAction(Constants::A_REACHABILITY, m_reachabilityAction);
    ActionManager::registerAction(Constants::A_RELOAD, m_reloadAction);
    ActionManager::registerAction(Constants::A_FIT_IN_VIEW, m_fitInViewAction);

    ActionManager::group(Constants::TOOLS_GROUP)->addAction(Constants::A_REACHABILITY);

    auto reachabilityToolbarGroup = ActionManager::createActionGroup(Constants::G_REACHABILITY_TOOLBAR);
    reachabilityToolbarGroup->addAction(Constants::A_FIT_IN_VIEW);
    reachabilityToolbarGroup->addSeparator();
    reachabilityToolbarGroup->addAction(Constants::A_RELOAD);

    EditorManager::registerEditorFactory(Constants::REACHABILITY_EDITOR, new ReachabilityTreeEditorFactory());

    connect(ApplicationProjectController::instance(), &ApplicationProjectController::editorOpened, this, &ReachabilityPlugin::onEditorOpened);
}

void ReachabilityPlugin::onEditorOpened(IEditor *editor) {
    // attach action
    if (editor->document()) {
        if (editor->document()->documentType() == Constants::PETRI_NET_DOC_TYPE) {
            auto action = ActionManager::action(Constants::A_REACHABILITY);
            if (action) {
                ActionManager::attachAction(action, editor->widget());
            }
        }
    }
}

void ReachabilityPlugin::onReachabilityTreeRequested() {
    QPointer project = ApplicationProjectController::currentProject();
    auto currentDocument = EditorManager::currentEditor()->document();
    // check if current document is a Petri Net document
    if (auto petriNetDocument = qobject_cast<PetriNetDocument*>(currentDocument)) {
        if (m_lockedDocuments.contains(petriNetDocument)) {
            return;
        }

        auto document = m_reachabilityDocuments.value(petriNetDocument, nullptr);
        if (!document) {
            auto task = new BuildReachabilityGraphTask(petriNetDocument->model()->net());
            connect(task, &BuildReachabilityGraphTask::graphComplete,  this, [this, project, petriNetDocument](ptn::modules::reachability::Reachability* graph) {
                auto document = new ReachabilityDocument();
                DocumentManager::addDocument(document);
                document->setGraph(graph);
                m_reachabilityDocuments.insert(petriNetDocument, document);
                EditorManager::openEditor(project, document);
            });
            m_lockedDocuments.insert(petriNetDocument);
            connect(task, &BuildReachabilityGraphTask::destroyed, this, [this, petriNetDocument]() {
                m_lockedDocuments.remove(petriNetDocument);
            });
            ApplicationProjectController::runBackgroundTask(project, task);
        } else {
            EditorManager::openEditor(project, document);
        }
    }
}

void ReachabilityPlugin::onReloadRequested() {
    QPointer project = ApplicationProjectController::currentProject();
    auto currentDocument = EditorManager::currentEditor()->document();
    if (auto reachabilityDocument = qobject_cast<ReachabilityDocument*>(currentDocument)) {
        auto petriNetDocument = m_reachabilityDocuments.key(reachabilityDocument, nullptr);
        if (!petriNetDocument) {
            qWarning() << "BUG: Reachability Document not found";
            return;
        }

        if (m_lockedDocuments.contains(petriNetDocument)) {
            return;
        }

        auto task = new BuildReachabilityGraphTask(petriNetDocument->model()->net());
        connect(task, &BuildReachabilityGraphTask::graphComplete,  this, [this, project, reachabilityDocument](ptn::modules::reachability::Reachability* graph) {
            reachabilityDocument->setGraph(graph);
        });

        m_lockedDocuments.insert(petriNetDocument);
        connect(task, &BuildReachabilityGraphTask::destroyed, this, [this, petriNetDocument]() {
            m_lockedDocuments.remove(petriNetDocument);
        });

        ApplicationProjectController::runBackgroundTask(project, task);
    }
}

void ReachabilityPlugin::onFitInView() {
    auto currentEditor = EditorManager::currentEditor();
    auto widget = qobject_cast<ReachabilityWindow*>(currentEditor->widget());
    if (widget) {
        widget->onFitInView();
    }
}
