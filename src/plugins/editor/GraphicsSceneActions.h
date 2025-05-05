// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_GRAPHICSSCENEACTIONS_H
#define GPNE_GRAPHICSSCENEACTIONS_H

#include <ptn/place.h>
#include <QObject>

class QAction;
class GraphicsScene;

class GraphicsSceneActions : public QObject {

public:
    explicit GraphicsSceneActions(GraphicsScene* scene);

    [[nodiscard]]
    QAction* undoAction() const
    {
        return m_undoAction;
    }

    [[nodiscard]]
    QAction* redoAction() const
    {
        return m_redoAction;
    }

    [[nodiscard]]
    QAction* hAlignmentAction() const
    {
        return m_hAlignment;
    }

    [[nodiscard]]
    QAction* vAlignmentAction() const
    {
        return m_vAlignment;
    }

    [[nodiscard]]
    QAction* layoutAction() const
    {
        return m_layoutAction;
    }

    [[nodiscard]]
    const QHash<ptn::net::vertex::VertexType, QList<QAction*>>& vertexAddActions();

private:
    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_hAlignment;
    QAction* m_vAlignment;
    QAction* m_layoutAction;
};

#endif // GPNE_GRAPHICSSCENEACTIONS_H
