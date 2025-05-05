// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "GraphicsViewZoom.h"
#include <QApplication>
#include <QtMath>
#include <QGraphicsView>
#include <QMouseEvent>

#define MAX_SCALE 5.f
#define MIN_SCALE 0.5

GraphicsViewZoom::GraphicsViewZoom(QGraphicsView* view) : QObject(view), m_view(view)
{
    m_view->viewport()->installEventFilter(this);
    m_view->setMouseTracking(true);
    m_modifiers = Qt::ControlModifier;

    m_zoom_factor_base = 1.0015;
    target_viewport_pos = {0, 0};
    target_scene_pos = {0, 0};

    m_max_scale = MAX_SCALE;
    m_min_scale = MIN_SCALE;
}

void GraphicsViewZoom::gentleZoom(double factor)
{
    QTransform transform = m_view->transform();
    auto scale = transform.m11() * factor;
    setScale(scale);
}

void GraphicsViewZoom::setScale(double scale) { setScale(scale, target_scene_pos); }

void GraphicsViewZoom::setScale(double scale, const QPointF& scenePos)
{
    QTransform transform = m_view->transform();
    qreal current = transform.m11();

    if (scale > m_max_scale) {
        transform.setMatrix(
            m_max_scale,
            transform.m12(),
            transform.m13(),
            transform.m21(),
            m_max_scale,
            transform.m23(),
            transform.m31(),
            transform.m32(),
            transform.m33()
        );
    } else if (scale < m_min_scale) {
        transform.setMatrix(
            m_min_scale,
            transform.m12(),
            transform.m13(),
            transform.m21(),
            m_min_scale,
            transform.m23(),
            transform.m31(),
            transform.m32(),
            transform.m33()
        );
    } else {
        transform.setMatrix(
            scale,
            transform.m12(),
            transform.m13(),
            transform.m21(),
            scale,
            transform.m23(),
            transform.m31(),
            transform.m32(),
            transform.m33()
        );
    }

    if (current == transform.m11()) // Fixed when scroll in max zoom
    {
        return;
    }

    target_viewport_pos = m_view->mapFromScene(scenePos);
    target_scene_pos = scenePos;

    m_view->setTransform(transform);
    m_view->centerOn(target_scene_pos);

    QPointF delta_viewport_pos =
        target_viewport_pos -
        QPointF(m_view->viewport()->width() / 2., m_view->viewport()->height() / 2.);
    QPointF viewport_center = m_view->mapFromScene(target_scene_pos) - delta_viewport_pos;
    m_view->centerOn(m_view->mapToScene(viewport_center.toPoint()));

    emit zoomed();
}

void GraphicsViewZoom::setModifier(Qt::KeyboardModifiers modifiers) { m_modifiers = modifiers; }

void GraphicsViewZoom::set_min_scale(double min_scale) {
    if (min_scale < 0 || min_scale > m_max_scale) {
        return;
    }

    m_min_scale = min_scale;
}

void GraphicsViewZoom::set_max_scale(double max_scale) {
    if (max_scale < 0 || max_scale < m_min_scale) {
        return;
    }

    m_max_scale = max_scale;
}

void GraphicsViewZoom::setZoomFactorBase(double value) { m_zoom_factor_base = value; }

bool GraphicsViewZoom::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::MouseMove) {
        auto mouse_event = dynamic_cast<QMouseEvent*>(event);
        QPointF delta = target_viewport_pos - mouse_event->pos();
        if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
            target_viewport_pos = mouse_event->pos();
            target_scene_pos = m_view->mapToScene(mouse_event->pos());
        }
    } else if (event->type() == QEvent::Wheel) {
        auto wheel_event = dynamic_cast<QWheelEvent*>(event);
        if (QApplication::keyboardModifiers() == m_modifiers && wheel_event->angleDelta().y() != 0)
        {
            double angle = wheel_event->angleDelta().y();
            double factor = qPow(m_zoom_factor_base, angle);
            gentleZoom(factor);
            return true;
        }
    } else if (event->type() == QEvent::NativeGesture) {
        auto nge = dynamic_cast<QNativeGestureEvent*>(event);
        if (nge) {
            if (nge->gestureType() == Qt::ZoomNativeGesture) {
                double angle = nge->delta().y();
                double factor = qPow(m_zoom_factor_base, angle);
                gentleZoom(factor);
                return true;
            }
        }
    }
    return false;
}
