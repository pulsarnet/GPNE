// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_GRAPHICSCENE_H
#define GPNE_GRAPHICSCENE_H

#include <QGraphicsScene>
#include <QSet>

class GraphicsSceneActions;
class IEdge;
class IVertex;
class NetModel;

class GraphicsScene : public QGraphicsScene {

    Q_OBJECT

public:
    explicit GraphicsScene(QObject* parent = nullptr);
    ~GraphicsScene() override;

    /********************* work with model ***************************/
    void setModel(NetModel* model);
    NetModel* model() const;
    void reload();

    // Operations with items
    IVertex* netItemAt(const QPointF& pos);

    [[nodiscard]]
    const QSet<IVertex*>& vertices() const;

    [[nodiscard]]
    const QSet<IEdge*>& edges() const;

    void dotVisualization(const char* algorithm);
    GraphicsSceneActions* actions();

    void removeAllModelItems();

public slots:

    void slotHorizontalAlignment(bool triggered);
    void slotVerticalAlignment(bool triggered);
    void onSelectionChanged();
    void onModelSelectionChanged();

    void onVertexInserted(IVertex*);
    void onVertexRemoved(IVertex*);

    void onEdgeInserted(IEdge*);
    void onEdgeRemoved(IEdge*);

public:
    void createActions();

    void drawBackground(QPainter* painter, const QRectF& rect) override;
private:
    QList<QMetaObject::Connection> m_objectConnections;
    GraphicsSceneActions* m_actions = nullptr;

    NetModel* m_model;

    QSet<IVertex*> m_vertices;
    QSet<IEdge*> m_edges;

    QPointF lastPos = QPointF(0, 0);
};

#endif // GPNE_GRAPHICSCENE_H
