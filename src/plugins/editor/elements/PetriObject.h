// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_PETRI_OBJECT_H
#define GPNE_PETRI_OBJECT_H

#include <ptn/types.h>
#include <QGraphicsItem>

class Edge;
class GraphicsScene;

class PetriObject : public QGraphicsItem {
protected:
    explicit PetriObject(QGraphicsItem* parent = nullptr);

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

public:
    virtual char symbol() const noexcept = 0;

    /**************** geometry ********************/
    virtual QPointF jointPoint(const QPointF& to) const = 0;
    virtual QPointF center() const = 0;
    virtual qreal angleBetween(const QPointF& to) const;

    ~PetriObject();

protected:
    /**************** label ********************/
    virtual usize displayId() const = 0;
    void updateLabel() const;
    void updateLabelPosition() const;

private:
    QGraphicsSimpleTextItem* m_name;
};

#endif // GPNE_PETRI_OBJECT_H
