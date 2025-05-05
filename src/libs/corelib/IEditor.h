// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef IEDITOR_H
#define IEDITOR_H

#include <QObject>
#include <QIcon>

class Project;
class IDocument;

class IEditor : public QObject {
    Q_OBJECT
public:

    IEditor(IDocument* document);

    virtual QString displayName() const = 0;

    virtual QIcon icon() const = 0;

    IDocument* document() const;

    QWidget* widget();

    bool isModified() const;

protected:

    virtual QWidget* editorWidget() = 0;

private:

    IDocument* m_document;

    QWidget* m_editorWidget = nullptr;

};

#endif //IEDITOR_H
