// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PETRINETEDITOR_H
#define PETRINETEDITOR_H

#include <corelib/IEditor.h>
#include <corelib/IEditorFactory.h>

class PetriNetEditor : public IEditor {
    Q_OBJECT
public:

    PetriNetEditor(IDocument* document);

    QString displayName() const override;

    QIcon icon() const override;

protected:

    QWidget * editorWidget() override;

};

class PetriNetEditorFactory : public IEditorFactory {
public:
    IEditor* createEditor(IDocument *document) override;

    bool isDocumentTypeSupported(const QString &documentType) const override;
};

#endif //PETRINETEDITOR_H
