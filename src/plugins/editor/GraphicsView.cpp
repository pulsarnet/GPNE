// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "GraphicsView.h"
#include "GraphicsScene.h"
#include "GraphicsSceneActions.h"
#include "GraphicsViewZoom.h"
#include "registry/IVertex.h"
#include <QMenu>
#include <QMouseEvent>

GraphicsView::GraphicsView(QWidget* parent) : QGraphicsView(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setAttribute(Qt::WA_TranslucentBackground, false);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);
    this->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    // this->setCacheMode(CacheBackground);
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    this->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);

    m_zoom = new GraphicsViewZoom(this);
    m_zoom->setModifier(Qt::NoModifier);

    setTransformationAnchor(QGraphicsView::NoAnchor);

    setContentsMargins(0, 0, 0, 0);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        m_origin = event->pos();
        auto icon = QPixmap(":/images/tools/move.svg");

        setCursor(QCursor(icon));
        setInteractive(false);
    } else if (event->button() == Qt::RightButton) {
        this->contextMenu();
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
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

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    setInteractive(true);
    setCursor(Qt::ArrowCursor);

    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::contextMenu()
{
    auto gScene = dynamic_cast<GraphicsScene*>(scene());

    QMenu menu;
    menu.deleteLater();
    menu.setWindowFlags(menu.windowFlags() | Qt::FramelessWindowHint);
    menu.setAttribute(Qt::WA_TranslucentBackground);
    menu.setAttribute(Qt::WA_DeleteOnClose);

    menu.addAction(gScene->actions()->hAlignmentAction());
    menu.addAction(gScene->actions()->vAlignmentAction());

    menu.addSeparator();

    menu.addAction(gScene->actions()->layoutAction());

    menu.exec(QCursor::pos());
}

GraphicsView::~GraphicsView() noexcept
{
   // delete m_actionGroup;
}
