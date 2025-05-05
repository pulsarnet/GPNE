// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "GraphicsSceneActions.h"
#include "GraphicsScene.h"
#include "registry/IEdge.h"
#include <QActionGroup>
#include <QMenu>
#include <QUndoStack>

GraphicsSceneActions::GraphicsSceneActions(GraphicsScene* scene)
    : m_undoAction(nullptr)
    , m_redoAction(nullptr)
{
    // m_undoAction = scene->undoStack()->createUndoAction(scene, tr("&Undo"));
    // m_undoAction->setShortcut(QKeySequence::Undo);
    //
    // m_redoAction = scene->undoStack()->createRedoAction(scene, tr("&Redo"));
    // m_redoAction->setShortcut(QKeySequence::Redo);

    bool hasSelection = !scene->selectedItems().empty();
    m_hAlignment = new QAction(tr("Horizontal alignment"));
    m_hAlignment->setEnabled(hasSelection);
    connect(
        m_hAlignment,
        &QAction::triggered,
        scene,
        &GraphicsScene::slotHorizontalAlignment
    ); // NOLINT(*-unused-return-value)

    m_vAlignment = new QAction(tr("Vertical alignment"));
    m_vAlignment->setEnabled(hasSelection);
    connect(
        m_vAlignment,
        &QAction::triggered,
        scene,
        &GraphicsScene::slotVerticalAlignment
    ); // NOLINT(*-unused-return-value)

    m_layoutAction = new QAction(tr("Layout"));
    m_layoutAction->setToolTip(tr("Use analog of dot layout"));
}
