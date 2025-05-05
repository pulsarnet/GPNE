// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <QMessageBox>

#include "EditorManager.h"
#include "ApplicationProjectController.h"
#include "DocumentManager.h"
#include "IDocument.h"
#include "IEditor.h"
#include "IEditorFactory.h"
#include "Project.h"
#include "MainWindow.h"

/// \brief A static hash table to store instances of EditorManager for each project.
static QHash<Project*, EditorManager*> instances;

/// \brief A static hash table to store editor factories identified by a unique string key.
static QHash<QString, IEditorFactory*> editorFactories;

/// \brief A static hash table to map editors to their corresponding projects.
static QHash<IEditor*, Project*> editorToProject;

/// \brief A pointer to the current editor.
IEditor* currEditor = nullptr;

/// \brief Retrieves the instance of EditorManager for a given project.
/// \param project The project for which to retrieve the EditorManager instance.
/// \return The EditorManager instance for the given project.
EditorManager* EditorManager::instance(Project *project) {
    auto manager = instances.value(project, nullptr);
    if (!manager) {
        manager = new EditorManager();
        instances.insert(project, manager);
    }
    return manager;
}

/// \brief Retrieves the list of documents for a given project.
/// \param project The project for which to retrieve the documents.
/// \return A list of documents for the given project.
QList<IDocument *> EditorManager::documents(Project *project) {
    if (!project) {
        return {};
    }
    return instance(project)->m_editorsMap.keys();
}

/// \brief Opens an editor for a given document in a given project.
/// \param project The project in which to open the editor.
/// \param document The document to open in the editor.
/// \param editorId The unique identifier of the editor to open.
/// \return The opened editor, or nullptr if the editor could not be opened.
IEditor* EditorManager::openEditor(Project *project, IDocument* document, QString editorId) {
    // check if file is opened in existing editor of editorId of current editor manager
    auto editors = getEditors(project, document);
    if (!editors.isEmpty()) {
        setCurrentEditor(editors.first());
        return editors.first();
    }

    // if editorId is empty, get first editorId from possible editors
    if (editorId.isEmpty()) {
        QList<QString> possibleEditors;
        QString docType(document->documentType());
        for (auto it = editorFactories.begin(); it != editorFactories.end(); ++it) {
            if (it.value()->isDocumentTypeSupported(docType)) {
                possibleEditors.append(it.key());
            }
        }
        if (possibleEditors.isEmpty()) {
            qDebug() << "No editor found for document type:" << docType;
            return nullptr;
        }
        editorId = possibleEditors.first();
    }

    // create editor with id
    auto factory = editorFactories.value(editorId, nullptr);
    if (!factory) {
        qDebug() << "No editor factory found for editorId:" << editorId;
        return nullptr;
    }
    auto editor = factory->createEditor(document);
    if (!editor) {
        qDebug() << "Failed to create editor for editorId:" << editorId;
        return nullptr;
    }

    auto instance = EditorManager::instance(project);
    instance->m_editors.append(editor);
    instance->m_editorsMap[document].append(editor);
    editorToProject.insert(editor, project);
    emit instance->editorOpened(editor);
    return editor;
}

/// \brief Closes a given editor.
/// \param editor The editor to close.
void EditorManager::closeEditor(IEditor *editor) {
    qDebug().nospace() << "EditorManager::closeEditor(" << editor << ")";
    auto instance = EditorManager::instance(ApplicationProjectController::currentProject());

    emit instance->editorClosed(editor);
    instance->m_editors.removeAll(editor);
    instance->m_editorsMap[editor->document()].removeAll(editor);

    if (currEditor == editor) {
        setCurrentEditor(nullptr);
    }
}

/// \brief Retrieves the list of editors for a given document in a given project.
/// \param project The project for which to retrieve the editors.
/// \param document The document for which to retrieve the editors.
/// \return A list of editors for the given document in the given project.
QList<IEditor*> EditorManager::getEditors(Project *project, IDocument* document) {
    auto instance = EditorManager::instance(project);
    return instance->m_editorsMap.value(document, {});
}

/// \brief Retrieves the project associated with a given editor.
/// \param editor The editor for which to retrieve the associated project.
/// \return The project associated with the given editor.
Project* EditorManager::getProject(IEditor *editor) {
    return editorToProject.value(editor, nullptr);
}

/// \brief Sets the current editor.
/// \param editor The editor to set as the current editor.
void EditorManager::setCurrentEditor(IEditor *editor) {
    qDebug().nospace() << "EditorManager::setCurrentEditor(" << editor << ")";
    if (currEditor == editor) {
        return;
    }
    currEditor = editor;

    // find editor project
    emit instance(ApplicationProjectController::currentProject())->currentEditorChanged(editor);
}

/// \brief Retrieves the current editor.
/// \return The current editor.
IEditor* EditorManager::currentEditor() {
    return currEditor;
}

/// \brief Registers an editor factory with a unique identifier.
/// \param editorId The unique identifier for the editor factory.
/// \param factory The editor factory to register.
void EditorManager::registerEditorFactory(const QString &editorId, IEditorFactory *factory) {
    if (!factory || editorId.isEmpty() || editorFactories.contains(editorId)) {
        return;
    }

    editorFactories.insert(editorId, factory);
}

/// \brief Unloads all editors and documents for a given project.
/// \param project The project for which to unload all editors and documents.
/// \return True if all editors and documents were successfully unloaded, false otherwise.
bool EditorManager::unloadAllEditorsAndDocuments(Project *project) {
    if (!project) {
        return true;
    }

    const auto instance = EditorManager::instance(project);

    std::vector<IDocument*> autoSave;
    std::vector<IDocument*> requestSave;
    for (auto [document, _] : instance->m_editorsMap.asKeyValueRange()) {
        if (document->isInternalDocument()) { // it is internal document
            autoSave.push_back(document);
        } else if (document->isModified()) {
            requestSave.push_back(document);
        }
    }

    if (!requestSave.empty()) {
        QString message = std::accumulate(requestSave.begin(), requestSave.end(), QString(),
            [](const QString& a, IDocument* b) {
                return a + "\n" + b->displayName();
        });
        const QMessageBox::StandardButton reply = QMessageBox::question(
            nullptr,
            QObject::tr("Save changes"),
            QObject::tr("There are unsaved changes in the following documents:%1\nDo you want to save them?").arg(message),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
        );

        if (reply == QMessageBox::Yes) {
            for (const auto document : requestSave) {
                QString errorString;
                document->save(FileRef(), errorString);
            }
        } else if (reply == QMessageBox::Cancel) {
            return false;
        }
    }

    for (const auto document : autoSave) {
        QString errorString;
        document->save(FileRef(), errorString);
    }

    for (const auto editor : instance->m_editors) {
        closeEditor(editor);
    }

    for (auto [document, _] : instance->m_editorsMap.asKeyValueRange()) {
        DocumentManager::removeDocument(document);
    }

    return true;
}

