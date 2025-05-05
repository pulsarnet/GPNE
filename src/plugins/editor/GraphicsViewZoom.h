// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_GRAPHICSVIEWZOOM_H
#define GPNE_GRAPHICSVIEWZOOM_H

#include <QObject>
#include <QPointF>

class QGraphicsView;

class GraphicsViewZoom : public QObject {

    Q_OBJECT

public:
    explicit GraphicsViewZoom(QGraphicsView* view);

    void gentleZoom(double factor);
    void setScale(double scale);
    void setScale(double scale, const QPointF& pos);

    void setModifier(Qt::KeyboardModifiers modifiers);

    double min_scale() const { return m_min_scale; }
    void set_min_scale(double min_scale);

    double max_scale() const { return m_max_scale; }
    void set_max_scale(double max_scale);

    void setZoomFactorBase(double value);

private:
    QGraphicsView* m_view;
    Qt::KeyboardModifiers m_modifiers;
    double m_zoom_factor_base;
    QPointF target_scene_pos, target_viewport_pos;
    double m_min_scale, m_max_scale;

    bool eventFilter(QObject* object, QEvent* event);

signals:

    void zoomed();
};

#endif // GPNE_GRAPHICSVIEWZOOM_H
