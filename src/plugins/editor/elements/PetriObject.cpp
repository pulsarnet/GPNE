// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "PetriObject.h"
#include "../GraphicsScene.h"
#include <ptn/net.h>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>

PetriObject::PetriObject(QGraphicsItem* parent) : QGraphicsItem(parent)
{
    m_name = new QGraphicsSimpleTextItem("", this);
    m_name->setFlags(m_name->flags() & 0);
    m_name->setCacheMode(DeviceCoordinateCache);
    m_name->setAcceptHoverEvents(false);

    setFlags(
        ItemIsMovable | ItemSendsGeometryChanges | ItemIsSelectable | ItemUsesExtendedStyleOption
    );
    setCacheMode(DeviceCoordinateCache);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
}

void PetriObject::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }

    QGraphicsItem::mousePressEvent(event);
}

void PetriObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void PetriObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

qreal PetriObject::angleBetween(const QPointF& to) const
{
    qreal x = center().x();
    qreal y = center().y();

    qreal deltaX = to.x() - x;
    qreal deltaY = to.y() - y;

    qreal rotation = -atan2(deltaX, deltaY);
    rotation = qDegreesToRadians(qRadiansToDegrees(rotation) + 180.0);

    return rotation;
}

void PetriObject::hoverMoveEvent(QGraphicsSceneHoverEvent*)
{
    QString toolTip;
    setToolTip(toolTip);
}

void PetriObject::updateLabel() const
{
    m_name->setText(QString::fromUtf8("%1%2").arg(symbol()).arg(displayId()));
    updateLabelPosition();
}

void PetriObject::updateLabelPosition() const
{
    QRectF nameRect = m_name->boundingRect();
    QRectF objectRect = boundingRect();

    auto offsetX = nameRect.width() / 2.;
    auto offsetY = objectRect.height() / 2. + nameRect.height();
    m_name->setPos(objectRect.center() + QPointF(-offsetX, -offsetY));
}

void PetriObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
}

PetriObject::~PetriObject() {}
