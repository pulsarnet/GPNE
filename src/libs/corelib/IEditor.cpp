// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "IEditor.h"

IEditor::IEditor(IDocument *document) : QObject(), m_document(document) {}

IDocument * IEditor::document() const {
    return m_document;
}

QWidget* IEditor::widget() {
    if (!m_editorWidget) {
        m_editorWidget = editorWidget();
    }
    return m_editorWidget;
}
