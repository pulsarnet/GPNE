// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef IEDGE_H
#define IEDGE_H

#include "IItem.h"
#include <QGraphicsItem>
#include <ptn/net.h>
#include <QJsonObject>
#include <QIcon>

class IVertex;

typedef std::pair<ptn::net::vertex::VertexType, usize> vertex_id;
typedef std::pair<vertex_id, vertex_id> edge_type;

class IEdge : public IItem, public QGraphicsLineItem {
public:
    enum Direction
    {
        PlaceToTransition = 0x1,
        TransitionToPlace = 0x2,
    };

    Q_DECLARE_FLAGS(Directions, Direction)

    virtual QByteArray typeId() const = 0;
    virtual QIcon icon() const = 0;

    virtual IEdge* create() const = 0;

    bool start(IVertex*);
    bool finish(IVertex*);

    IVertex* begin() const;
    IVertex* end() const;

    IVertex* place() const { return m_place; }

    IVertex* transition() const { return m_transition; }

    Directions directions() const { return m_directions; }

    void setDirections(Directions direction);
    virtual bool allowDirection(Direction direction) const = 0;
    bool isBidirectional() const;

    virtual bool hasWeight() const = 0;
    void setWeight(uint32_t newWeight, Direction direction);

    uint32_t weight(Direction direction) const
    {
        return m_directions.testFlag(direction) ? m_weight[direction == PlaceToTransition ? 0 : 1]
                                                : 0;
    }

    void onWeightChanged();

    IEdge* asIEdge() { return this; }

    QGraphicsItem* asGraphicsItem() { return this; }

    // QGraphicsLineItem
    QPainterPath shape() const override;
    QRectF boundingRect() const final;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) final;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) final;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) final;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) final;

    void moveTo(QPointF point);
    void updatePosition();
    virtual QPainterPath arrowShape(QLineF) = 0;

    // model
    virtual void emitDataChanged();
    // restore
    virtual QList<edge_type> getEdgePairs(ptn::net::PetriNet* m_net) = 0;

    virtual bool loadFromJson(const QJsonObject&);
    virtual QJsonObject toJson() const;

    ~IEdge() override;

protected:
    IEdge();

    void updateDrawLine();
    void updateLabel();

    friend class NetModel;

private:
    IVertex* m_place = nullptr;
    IVertex* m_transition = nullptr;

    QString m_text;
    QTransform m_textTransform;
    QLineF m_drawLine;
    QLineF m_selectionLine;

    Directions m_directions = Directions::fromInt(0);
    uint32_t m_weight[2] = {1, 1};

    QPainterPath m_arrowShape[2];
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IEdge::Directions)

#endif // IEDGE_H
