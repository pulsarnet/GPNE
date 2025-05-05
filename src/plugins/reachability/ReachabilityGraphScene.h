// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_REACHABILITY_TREE_SCENE_H
#define GPNE_REACHABILITY_TREE_SCENE_H

/// ReachabilityGraphScene does not free up memory rust::Reachability

#include <ptn/reachability.h>
#include <QGraphicsScene>

class ReachabilityLine;
class ReachabilityNode;
class Graph;

namespace ptn::modules::reachability
{
    struct ReachabilityGraph;
}

class ReachabilityGraphScene : public QGraphicsScene {

public:
    explicit ReachabilityGraphScene(QObject* parent = nullptr);

    void drawBackground(QPainter* painter, const QRectF& rect) override;
    ReachabilityNode* node_at(size_t idx) const;

    ~ReachabilityGraphScene() override;

    void addNode(
        QList<int32_t> data,
        ptn::modules::reachability::CovType type,
        const QList<ptn::net::vertex::VertexIndex>& headers
    );
    void
    addEdge(ReachabilityNode* from, ReachabilityNode* to, ptn::net::vertex::VertexIndex transition);
    void removeAll();
    void updateLayout();

    const QList<ReachabilityNode*>& nodes() const { return m_nodes; }

private:
    QList<ReachabilityNode*> m_nodes;
    QList<ReachabilityLine*> m_edges;

    Graph* m_graph;
};

#endif // GPNE_REACHABILITY_TREE_SCENE_H
