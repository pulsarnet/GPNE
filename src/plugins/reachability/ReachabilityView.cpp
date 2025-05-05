// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ReachabilityView.h"

#include <QMouseEvent>
#include <editor/GraphicsViewZoom.h>
#include "ReachabilityGraphScene.h"
#include "ReachabilityNode.h"

ReachabilityView::ReachabilityView(QWidget* parent) : QGraphicsView(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);
    this->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setOptimizationFlags(DontAdjustForAntialiasing);
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    setTransformationAnchor(QGraphicsView::NoAnchor);

    m_zoom = new GraphicsViewZoom(this);
    m_zoom->setModifier(Qt::NoModifier);
    m_zoom->set_min_scale(0.01f);
}

void ReachabilityView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        m_origin = event->pos();
        auto icon = QPixmap(":/images/tools/move.svg");
        setCursor(QCursor(icon));
        setInteractive(false);
    }
    QGraphicsView::mousePressEvent(event);
}

void ReachabilityView::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::MiddleButton) {
        QPointF oldp = mapToScene(m_origin.toPoint());
        QPointF newp = mapToScene(event->pos());
        QPointF translation = newp - oldp;

        translate(translation.x(), translation.y());

        m_origin = event->pos();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void ReachabilityView::mouseReleaseEvent(QMouseEvent* event)
{
    setInteractive(true);
    setCursor(Qt::ArrowCursor);
    QGraphicsView::mouseReleaseEvent(event);
}

void ReachabilityView::fitInView()
{
    if (!scene()) {
        return;
    }

    ReachabilityNode* first = dynamic_cast<ReachabilityGraphScene*>(scene())->nodes().first();
    if (first) {
        QRectF itemsRect = first->mapRectToScene(first->boundingRect());
        qDebug() << itemsRect << scene()->sceneRect();
        QGraphicsView::fitInView(itemsRect, Qt::KeepAspectRatio);
        m_zoom->setScale(1, itemsRect.center());
    }
}

