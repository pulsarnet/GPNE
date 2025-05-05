// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <QHash>
#include <QObject>

class IEditorFactory;
class IDocument;
class IEditor;
class Project;

class EditorManager : public QObject {

    Q_OBJECT

public:

    static EditorManager* instance(Project* project);

    static QList<IDocument*> documents(Project* project);

    static IEditor* openEditor(Project* project, IDocument* document, QString editorId = QString());
    static void closeEditor(IEditor* editor);

    static QList<IEditor*> getEditors(Project* project, IDocument* document);
    static Project* getProject(IEditor* editor);

    static void setCurrentEditor(IEditor* editor);

    static IEditor* currentEditor();

    static void registerEditorFactory(const QString& editorId, IEditorFactory* factory);

    static bool closeAllDocuments(Project* project, bool ask = true);
    static bool unloadAllEditorsAndDocuments(Project* project);

signals:

    void editorOpened(IEditor* editor);
    void currentEditorChanged(IEditor* editor);
    void editorClosed(IEditor* editor);

private:

    QList<IEditor*> m_editors;

    // Store editors per document
    // Document erase only if project is closed
    QHash<IDocument*, QList<IEditor*>> m_editorsMap;

};

#endif //EDITORMANAGER_H
