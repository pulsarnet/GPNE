// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef EDITORTOOLCONTROLLER_H
#define EDITORTOOLCONTROLLER_H

#include <QObject>

class GraphicsScene;
class PetriObject;
class IVertex;
class IEdge;
class QGraphicsSceneMouseEvent;
class GraphicsView;
class QActionGroup;
class EditorWidget;


class EditorToolController final : public QObject {

    Q_OBJECT

    enum Mode
    {
        A_Add = 1 << 0,
        A_Connection = 1 << 2,
        A_Move = 1 << 3,
        A_Remove = 1 << 4,
        A_Rotation = 1 << 5,
        A_Nothing = 1 << 6,
        A_Marker = 1 << 7,
    };

public:
    explicit EditorToolController(GraphicsView* view, QObject* parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;

protected:

    void createActions();

    void setMode(Mode mode);

    bool onMousePressEvent(QGraphicsSceneMouseEvent* event);
    bool onMouseMoveEvent(QGraphicsSceneMouseEvent* event);
    bool onMouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    void insertVertex(QGraphicsSceneMouseEvent* event);
    void removeObject(QGraphicsSceneMouseEvent* event);

    void edgeStart(QGraphicsSceneMouseEvent* event);
    void edgeCommit(QGraphicsSceneMouseEvent* event);
    void edgeRollback();

private:

    GraphicsView* m_view;
    GraphicsScene* m_scene;
    QActionGroup* m_actionGroup;


    bool m_dragInProgress = false;
    QList<QPair<PetriObject*, QPointF>> m_draggedItems;

    IEdge* m_currentConnection = nullptr;
    IEdge* m_edgeFactory = nullptr;
    IVertex* m_vertexFactory = nullptr;

    Mode m_mode;

    bool m_inManualSend = false;

};

#endif //EDITORTOOLCONTROLLER_H
