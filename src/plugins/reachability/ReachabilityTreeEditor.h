// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef REACHABILITYTREEEDITOR_H
#define REACHABILITYTREEEDITOR_H

#include <QHash>
#include <corelib/IEditor.h>
#include <corelib/IEditorFactory.h>

class ReachabilityGraphScene;

class ReachabilityTreeEditor : public IEditor {
    Q_OBJECT
public:
    explicit ReachabilityTreeEditor(IDocument* document);

    QString displayName() const override;

    QIcon icon() const override;

protected:
    QWidget* editorWidget() override;

private:


};

class ReachabilityTreeEditorFactory : public IEditorFactory {
public:
    IEditor* createEditor(IDocument *document) override;

    bool isDocumentTypeSupported(const QString &documentType) const override;

private:

    QHash<IDocument*, ReachabilityGraphScene*> m_scenes;
};


#endif //REACHABILITYTREEEDITOR_H
