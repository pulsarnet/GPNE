// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef IEDITORFACTORY_H
#define IEDITORFACTORY_H

class IEditor;
class IDocument;

class IEditorFactory {
public:
    virtual ~IEditorFactory() = default;

    virtual IEditor* createEditor(IDocument* document) = 0;

    virtual bool isDocumentTypeSupported(const QString& documentType) const = 0;
};

#endif //IEDITORFACTORY_H
