// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ReachabilityGraphScene.h"

#include <fstream>

#include <corelib/viz/Buhheim.h>
#include <corelib/viz/Graph.h>
#include "ReachabilityLine.h"
#include "ReachabilityNode.h"

#include <iostream>
#include <QEvent>
#include <QPainter>
#include <QProgressBar>

ReachabilityGraphScene::ReachabilityGraphScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_graph(nullptr)
{
    setSceneRect(-200, -200, 200, 200);
}

void ReachabilityGraphScene::addNode(
    QList<int32_t> data,
    ptn::modules::reachability::CovType type,
    const QList<ptn::net::vertex::VertexIndex>& headers
)
{
    const auto node = new ReachabilityNode(std::move(data), headers);

    // const auto index = QString("s%1").arg(m_nodes.length());
    const auto name = QString("S%1").arg(m_nodes.count());
    // node->setLabel(name);
    node->setCovType(type);

    m_nodes.push_back(node);
    addItem(node);
}

void ReachabilityGraphScene::addEdge(
    ReachabilityNode* from,
    ReachabilityNode* to,
    ptn::net::vertex::VertexIndex transition
)
{
    if (!from || !to) {
        return;
    }

    auto graph_edge = new ReachabilityLine(QString("T%1").arg(transition.id), from, to);
    m_edges.push_back(graph_edge);
    addItem(graph_edge);
}

void ReachabilityGraphScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    // todo we can take a sceneRect and a viewportRect and draw a grid inside the visible part
    double gridSize = 50.;
    double viewScale = painter->transform().m11();
    double spacingInScene = gridSize / viewScale;

    double base = std::pow(10, std::floor(std::log10(spacingInScene)));
    double frac = spacingInScene / base;

    double niceFrac;
    if (frac < 1.5) {
        niceFrac = 1;
    } else if (frac < 3) {
        niceFrac = 2;
    } else if (frac < 7) {
        niceFrac = 5;
    } else {
        niceFrac = 10;
    }

    gridSize = niceFrac * base;

    painter->setClipRect(sceneRect());
    QGraphicsScene::drawBackground(painter, rect);

    QRectF sRect(sceneRect());
    qreal left = int(rect.left()) - (int(rect.left()) % (int)gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % (int)gridSize);

    QVector<QLineF> lines;
    lines.reserve(100);
    qreal x = left;
    while (x <= rect.right()) {
        lines.append(QLineF(QPointF(x, sRect.top()), QPointF(x, sRect.bottom())));
        x += gridSize;
    }

    qreal y = top;
    while (y <= rect.bottom()) {
        lines.append(QLineF(QPointF(sRect.left(), y), QPointF(sRect.right(), y)));

        y += gridSize;
    }

    QPen dotPen(Qt::lightGray, 0);
    dotPen.setDashPattern({1.0, 8.0});
    painter->setPen(dotPen);
    painter->drawLines(lines);
}

ReachabilityNode * ReachabilityGraphScene::node_at(size_t idx) const {
    return m_nodes.at(idx);
}

void ReachabilityGraphScene::removeAll()
{
    clear();
    m_nodes.clear();
    m_edges.clear();

    delete m_graph;
    m_graph = nullptr;
}

void ReachabilityGraphScene::updateLayout() {
    if (!m_graph) {
        m_graph = new Graph;
        QSet<ReachabilityNode*> treeNodes;
        // add for empty
        if (!m_nodes.empty()) {
            Node* node = m_graph->addNode();
            m_nodes.first()->setDrawNode(node);
            treeNodes.insert(m_nodes.first());
        }

        for (auto edge : m_edges) {
            auto itFirst = treeNodes.find(edge->first());
            if (itFirst == treeNodes.end()) {
                Node* node = m_graph->addNode();
                edge->first()->setDrawNode(node);
                treeNodes.insert(edge->first());
            }

            auto itLast = treeNodes.find(edge->last());
            if (itLast == treeNodes.end()) {
                Node* node = m_graph->addNode();
                edge->last()->setDrawNode(node);
                edge->last()->drawNode()->addParent(edge->first()->drawNode());
                treeNodes.insert(edge->last());
            }
        }
    }

    Node* root = m_nodes[0]->drawNode();
    buchheim(root);

    // Update all nodes and edges
    std::for_each(m_nodes.begin(), m_nodes.end(), [](ReachabilityNode* node) {
        node->updateLayout();
    });

    std::for_each(m_edges.begin(), m_edges.end(), [](ReachabilityLine* edge) {
        edge->updateLayout();
    });

    QRectF bounds = itemsBoundingRect();
    const qreal margin = 20.0;
    bounds.adjust(-margin, -margin, margin, margin);
    setSceneRect(bounds);
}

ReachabilityGraphScene::~ReachabilityGraphScene() { delete m_graph; }
