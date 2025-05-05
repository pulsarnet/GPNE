// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_GRAPHICSVIEW_H
#define GPNE_GRAPHICSVIEW_H

#include <QGraphicsView>

class GraphicsViewZoom;

class GraphicsView : public QGraphicsView {

    Q_OBJECT

public:

    explicit GraphicsView(QWidget* parent = nullptr);
    virtual ~GraphicsView() noexcept;


protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void contextMenu();

private:
    GraphicsViewZoom* m_zoom = nullptr;
    QPointF m_origin;
};

#endif // GPNE_GRAPHICSVIEW_H
