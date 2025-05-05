// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SYNTHESISEDITOR_H
#define SYNTHESISEDITOR_H

#include <corelib/IEditor.h>
#include <corelib/IEditorFactory.h>

class SynthesisEditor : public IEditor {

    Q_OBJECT

public:
    SynthesisEditor(IDocument* document);

    QString displayName() const override;

    QIcon icon() const override;

protected:
    QWidget* editorWidget() override;
};

class SynthesisEditorFactory : public IEditorFactory {
public:
    IEditor* createEditor(IDocument *document) override;

    bool isDocumentTypeSupported(const QString &documentType) const override;
};

#endif //SYNTHESISEDITOR_H
