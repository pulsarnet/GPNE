// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "GraphicsScene.h"
#include "elements/PetriObject.h"
#include "GraphicsSceneActions.h"
#include "model/NetModel.h"
#include "registry/IEdge.h"
#include "registry/IVertex.h"
#include <ptn/net.h>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include <QJsonDocument>
#include <QJsonValue>
#include <QLineEdit>
#include <QPainter>

GraphicsScene::GraphicsScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_model(nullptr)
{
    setSceneRect(-12500, -12500, 25000, 25000);
    setBackgroundBrush(Qt::NoBrush);
    connect(this, &QGraphicsScene::selectionChanged, this, &GraphicsScene::onSelectionChanged);
}

GraphicsScene::~GraphicsScene() { removeAllModelItems(); }

void GraphicsScene::setModel(NetModel* model)
{
    if (m_model == model) {
        return;
    }

    if (m_model) {
        for (const QMetaObject::Connection& connection : m_objectConnections) {
            disconnect(connection);
        }
    }

    // clear selection before model reset but after disconnect
    clearSelection();

    m_model = model;
    reload();
    if (m_model) {
        m_objectConnections = {
            connect(m_model, &NetModel::vertexInserted, this, &GraphicsScene::onVertexInserted),
            connect(m_model, &NetModel::vertexRemoved, this, &GraphicsScene::onVertexRemoved),

            connect(m_model, &NetModel::edgeInserted, this, &GraphicsScene::onEdgeInserted),
            connect(m_model, &NetModel::edgeRemoved, this, &GraphicsScene::onEdgeRemoved),

            connect(
                m_model,
                &NetModel::selectionChanged,
                this,
                &GraphicsScene::onModelSelectionChanged
            ),
        };
    }
}

NetModel* GraphicsScene::model() const { return m_model; }

void GraphicsScene::reload()
{
    removeAllModelItems();

    if (!m_model) {
        return;
    }

    for (auto vertex : m_model->vertices()) {
        addItem(vertex);
        m_vertices.insert(vertex);
    }

    for (auto edge : m_model->edges()) {
        addItem(edge);
        m_edges.insert(edge);
    }
}

void GraphicsScene::removeAllModelItems()
{
    // remove ownership before delete
    for (auto edge : m_edges) {
        removeItem(edge);
    }

    for (auto vertex : m_vertices) {
        removeItem(vertex);
    }

    m_edges.clear();
    m_vertices.clear();
}

void GraphicsScene::onSelectionChanged()
{
    auto selected = selectedItems();
    QList<IVertex*> selectedVertices;
    QList<IEdge*> selectedEdges;
    for (auto item : selected) {
        if (auto vertex = dynamic_cast<IVertex*>(item); vertex) {
            selectedVertices.append(vertex);
        } else if (auto edge = dynamic_cast<IEdge*>(item); edge) {
            selectedEdges.append(edge);
        }
    }

    // TODO: need optimization. double call signals
    m_model->setSelectedVertices(selectedVertices);
    m_model->setSelectedEdges(selectedEdges);
}

void GraphicsScene::onModelSelectionChanged()
{
    blockSignals(true);
    clearSelection();
    for (auto item : m_model->selectedVertices()) {
        if (auto it = m_vertices.find(item); it != m_vertices.end()) {
            (*it)->setSelected(true);
        } else {
            qDebug() << "Trying select not presented vertex";
        }
    }
    for (auto item : m_model->selectedEdges()) {
        if (auto it = m_edges.find(item); it != m_edges.end()) {
            (*it)->setSelected(true);
        } else {
            qDebug() << "Trying select not presented edge";
        }
    }
    blockSignals(false);
}

void GraphicsScene::onVertexInserted(IVertex* vertex)
{
    m_vertices.insert(vertex);
    addItem(vertex->asGraphicsItem());
}

void GraphicsScene::onVertexRemoved(IVertex* vertex)
{
    removeItem(vertex);
    m_vertices.remove(vertex);
}

void GraphicsScene::onEdgeInserted(IEdge* edge)
{
    m_edges.insert(edge);
    addItem(edge->asGraphicsItem());
}

void GraphicsScene::onEdgeRemoved(IEdge* edge)
{
    removeItem(edge);
    m_edges.remove(edge);
}

IVertex* GraphicsScene::netItemAt(const QPointF& pos)
{
    qDebug() << m_vertices.size();
    auto it = std::find_if(m_vertices.begin(), m_vertices.end(), [&](IVertex* item) {
        return item->sceneBoundingRect().contains(pos);
    });

    return it != m_vertices.end() ? *it : nullptr;
}

const QSet<IVertex *> & GraphicsScene::vertices() const {
    return m_vertices;
}

const QSet<IEdge *> & GraphicsScene::edges() const {
    return m_edges;
}

void GraphicsScene::slotHorizontalAlignment(bool triggered)
{
    Q_UNUSED(triggered)

    qreal y = 0;
    int elements = 0;

    for (auto item : selectedItems()) {
        if (dynamic_cast<PetriObject*>(item)) {
            y += item->pos().y();
            elements++;
        }
    }
}

void GraphicsScene::slotVerticalAlignment(bool triggered)
{
    Q_UNUSED(triggered)

    qreal x = 0;
    int elements = 0;

    for (auto item : selectedItems()) {
        if (dynamic_cast<PetriObject*>(item)) {
            x += item->pos().x();
            elements++;
        }
    }
}


void GraphicsScene::dotVisualization(const char* algorithm)
{
    Q_UNUSED(algorithm)
    // TODO: replace graphviz
    // auto res = graph.build(algorithm);
    // for (auto& element : res.elements) {
    //     auto vertex = nodeToVertex.find(element.first).value();
    //     vertex->setPos(element.second);
    // }
}

void GraphicsScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    const int gridSize = 50.;
    QGraphicsScene::drawBackground(painter, rect);

    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVector<QLineF> lines;
    lines.reserve(100);
    qreal x = left;
    while (x <= rect.right()) {
        lines.append(QLineF(QPointF(x, rect.top()), QPointF(x, rect.bottom())));

        x += gridSize;
    }

    qreal y = top;
    while (y <= rect.bottom()) {
        lines.append(QLineF(QPointF(rect.left(), y), QPointF(rect.right(), y)));

        y += gridSize;
    }

    painter->setPen(QPen(Qt::lightGray, 0));
    painter->drawLines(lines);
}

GraphicsSceneActions* GraphicsScene::actions()
{
    if (!m_actions) {
        createActions();
    }

    return m_actions;
}

void GraphicsScene::createActions() { m_actions = new GraphicsSceneActions(this); }
