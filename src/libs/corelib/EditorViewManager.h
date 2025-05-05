// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_EDITORVIEWMANAGER_H
#define GPNE_EDITORVIEWMANAGER_H

#include <QObject>

class QTabWidget;
class IEditor;

class EditorViewManager : public QObject {

    Q_OBJECT

public:
    EditorViewManager();

    QWidget* widget() const;

public slots:

    void openEditor(IEditor* editor);
    void setCurrentEditor(IEditor* editor);
    void closeEditor(IEditor* editor);

    void onTabChanged(int index);
    void onTabCloseRequested(int index);

private:

    QTabWidget* m_tabWidget;
    QHash<IEditor*, QWidget*> m_editors;
};

#endif // GPNE_EDITORVIEWMANAGER_H
