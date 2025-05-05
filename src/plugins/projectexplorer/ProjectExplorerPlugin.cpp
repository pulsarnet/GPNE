// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ProjectExplorerPlugin.h"

#include <QDir>
#include <corelib/ApplicationProjectController.h>
#include <corelib/DockWindowManager.h>
#include <corelib/ActionManager.h>
#include <corelib/Constants.h>
#include <corelib/DocumentManager.h>
#include <corelib/EditorManager.h>
#include <corelib/IEditor.h>
#include <editor/Constants.h>
#include <editor/PetriNetDocument.h>
#include <editor/PetriNetEditor.h>

#include "Constants.h"
#include "ProjectExplorerView.h"
#include "ProjectFileIndexer.h"
#include "NewNetDialog.h"

void ProjectExplorerPlugin::initialize() {
    ProjectFileIndexer::addRootSubdir("nets"); // store nets in the "nets" directory

    DockWindowManager::registerDockWindowFactory("ProjectExplorer", new ProjectExplorerViewFactory());

    m_newNetAction = new QAction(tr("New Net"), this);
    ActionManager::registerAction(Constants::NEW_NET_ACTION, m_newNetAction);
    ActionManager::group(Constants::FILE_GROUP)->addAction(Constants::NEW_NET_ACTION, Constants::A_NEW_PROJECT);
    connect(m_newNetAction, &QAction::triggered, this, &ProjectExplorerPlugin::onNewNetAction);


    m_saveNetAction = new QAction(tr("Save Net"), this);
    ActionManager::registerAction(Constants::SAVE_NET_ACTION, m_saveNetAction);
    ActionManager::group(Constants::FILE_GROUP)->addAction(Constants::SAVE_NET_ACTION, Constants::A_SAVE_PROJECT);
    connect(m_saveNetAction, &QAction::triggered, this, &ProjectExplorerPlugin::onSaveNetAction);

    connect(ApplicationProjectController::instance(), &ApplicationProjectController::editorOpened, this, &ProjectExplorerPlugin::onEditorOpened);
}

void ProjectExplorerPlugin::onEditorOpened(IEditor* editor) {
    if (auto pneditor = qobject_cast<PetriNetEditor*>(editor)) {
        ActionManager::attachAction(Constants::SAVE_NET_ACTION, pneditor->widget());
    }
}

void ProjectExplorerPlugin::onNewNetAction() {
    Project* project = ApplicationProjectController::currentProject();

    NewNetDialog dialog(project);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QDir projectDir(project->root());
    QString name = dialog.name();
    FileRef file = FileRef(projectDir.absoluteFilePath(QString(Constants::NET_DIR) + "/" + name + "." + Constants::NET_EXTENSION));

    auto document = new PetriNetDocument();
    qDebug() << "Creating new document with filename:" << file.absolutePath();
    document->setFile(file);
    DocumentManager::addDocument(document);
    EditorManager::openEditor(project, document);
}

void ProjectExplorerPlugin::onSaveNetAction() {
    auto editor = EditorManager::currentEditor();
    if (editor) {
        auto document = editor->document();
        if (document->documentType() != Constants::PETRI_NET_DOC_TYPE) {
            qDebug() << "Document type is not supported for saving.";
            return;
        }

        QString errorString;
        if (!document->save(FileRef(), errorString)) {
            qDebug() << "Failed to save document:" << errorString;
        }
    }
}
