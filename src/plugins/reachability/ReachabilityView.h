// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_REACHABILITY_VIEW_H
#define GPNE_REACHABILITY_VIEW_H

#include <QGraphicsView>

class GraphicsViewZoom;

class ReachabilityView : public QGraphicsView {

public:
    explicit ReachabilityView(QWidget* parent = nullptr);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void fitInView();

private:
    QPointF m_origin;
    GraphicsViewZoom* m_zoom;
};

#endif // GPNE_REACHABILITY_VIEW_H
