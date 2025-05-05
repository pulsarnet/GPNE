// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <QTabBar>
#include <QTabWidget>
#include "EditorViewManager.h"
#include "EditorManager.h"
#include "IEditor.h"

/// \brief Constructor for EditorViewManager.
/// Initializes the tab widget and connects signals to slots.
EditorViewManager::EditorViewManager()
{
    m_tabWidget = new QTabWidget;
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);

    connect(
        m_tabWidget,
        &QTabWidget::currentChanged,
        this,
        &EditorViewManager::onTabChanged
    );

    connect(
        m_tabWidget,
        &QTabWidget::tabCloseRequested,
        this,
        &EditorViewManager::onTabCloseRequested
    );
}

/// \brief Retrieves the tab widget.
/// \return A pointer to the tab widget.
QWidget* EditorViewManager::widget() const
{
    return m_tabWidget;
}

/// \brief Opens an editor and adds it to the tab widget.
/// \param editor The editor to be opened.
void EditorViewManager::openEditor(IEditor* editor)
{
    qDebug() << "EditorViewManager::openEditor" << editor->displayName() << sender();
    if (!m_editors.contains(editor)) {
        auto widget = editor->widget();
        m_editors.insert(editor, widget);
        m_tabWidget->addTab(widget, editor->icon(), editor->displayName());
    }
}

/// \brief Sets the current editor in the tab widget.
/// \param editor The editor to be set as current.
void EditorViewManager::setCurrentEditor(IEditor *editor) {
    qDebug() << "EditorViewManager::setCurrentEditor" << editor << sender();
    if (!editor) {
        m_tabWidget->setCurrentIndex(-1);
        return;
    }

    if (m_editors.contains(editor)) {
        auto widget = m_editors.value(editor);
        int index = m_tabWidget->indexOf(widget);
        if (index >= 0) {
            m_tabWidget->setCurrentWidget(widget);
        } else {
            int idx = m_tabWidget->addTab(widget, editor->icon(), editor->displayName());
            m_tabWidget->setCurrentIndex(idx);
        }
        widget->setFocus();
    } else {
        qWarning() << "BUG: Editor not found in editor manager" << editor;
    }
}

/// \brief Closes an editor and removes it from the tab widget.
/// \param editor The editor to be closed.
void EditorViewManager::closeEditor(IEditor *editor) {
    qDebug() << "EditorViewManager::closeEditor" << editor->displayName() << sender();
    if (m_editors.contains(editor)) {
        auto widget = m_editors.value(editor);
        int index = m_tabWidget->indexOf(widget);
        if (index >= 0) {
            m_tabWidget->removeTab(index);
        }
        m_editors.remove(editor);
    }
}

/// \brief Slot called when the current tab is changed.
/// \param index The index of the new current tab.
void EditorViewManager::onTabChanged(int index) {
    qDebug() << "EditorViewManager::onTabChanged" << index << sender();
    // set current editor in editor manager
    if (index < 0) {
        EditorManager::setCurrentEditor(nullptr);
        return;
    }

    if (index >= m_tabWidget->count()) {
        EditorManager::setCurrentEditor(nullptr);
        return;
    }

    auto editor = m_editors.key(m_tabWidget->widget(index));
    EditorManager::setCurrentEditor(editor);
}

/// \brief Slot called when a tab close is requested.
/// \param index The index of the tab to be closed.
void EditorViewManager::onTabCloseRequested(int index) {
    qDebug() << "EditorViewManager::onTabCloseRequested" << index << sender();
    if (index < 0 || index >= m_tabWidget->count()) {
        return;
    }

    m_tabWidget->removeTab(index);
}
