// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "EditorToolController.h"
#include <QActionGroup>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QCoreApplication>
#include "GraphicsScene.h"
#include "GraphicsView.h"
#include "registry/IEdge.h"
#include "registry/IVertex.h"
#include "registry/Registry.h"
#include "toolbox/ToolBox.h"

QAction* makeAction(
    const QIcon& icon,
    const QString& name,
    bool checkable,
    const QVariant& data_,
    QActionGroup* actionGroup_,
    const QKeySequence& sequence,
    const QString& tooltip
)
{
    Q_ASSERT(actionGroup_);
    auto action = actionGroup_->addAction(icon, name);
    action->setShortcut(sequence);
    action->setCheckable(checkable);
    action->setData(data_);
    action->setToolTip(tooltip);
    return action;
}

EditorToolController::EditorToolController(GraphicsView *view, QObject *parent)
    : QObject(parent)
    , m_view(view)
    , m_scene(qobject_cast<GraphicsScene*>(view->scene()))
    , m_actionGroup(nullptr)
    , m_mode(A_Nothing)
{
    createActions();
    view->scene()->installEventFilter(this);
}

bool EditorToolController::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_view->scene()) {
        if (m_inManualSend) {
            return false;
        }

        bool result = false;
        m_inManualSend = true;
        switch (event->type()) {
            case QEvent::GraphicsSceneMousePress:
                result = onMousePressEvent(static_cast<QGraphicsSceneMouseEvent*>(event));
                break;
            case QEvent::GraphicsSceneMouseMove:
                result = onMouseMoveEvent(static_cast<QGraphicsSceneMouseEvent*>(event));
                break;
            case QEvent::GraphicsSceneMouseRelease:
                result = onMouseReleaseEvent(static_cast<QGraphicsSceneMouseEvent*>(event));
                break;
            default:
                break;
        }
        m_inManualSend = false;
        return result;
    }
    return QObject::eventFilter(watched, event);
}

void EditorToolController::createActions() {
    auto toolbar = new ToolBox((QWidget*)m_view);
    toolbar->setVisible(true);
    toolbar->setToolArea(ToolBox::TopLeft);
    toolbar->setButtonSize(QSize(40, 40));

    m_actionGroup = new QActionGroup(toolbar);

    int sequence = 1;
    for (auto vertex : Registry::vertices().values()) {
        auto action = new QAction(vertex->icon(), vertex->typeId());
        action->setCheckable(true);
        action->setShortcut(
            QKeySequence::fromString(QString::fromStdString(std::format("Shift+{}", sequence++)))
        );
        action->setActionGroup(m_actionGroup);
        connect(action, &QAction::triggered, [vertex, this](bool) {
            m_mode = A_Add;
            m_vertexFactory = vertex;
        });

        toolbar->addTool(action);
    }

    QList<QAction*> edgesActions;
    for (auto edge : Registry::edges().values()) {
        auto action = new QAction(edge->icon(), edge->typeId());
        action->setCheckable(true);
        action->setShortcut(
            QKeySequence::fromString(QString::fromStdString(std::format("Shift+{}", sequence++)))
        );
        action->setActionGroup(m_actionGroup);
        connect(action, &QAction::triggered, [edge, this](bool) {
            m_mode = A_Connection;
            m_edgeFactory = edge;
        });
        edgesActions.append(action);
    }

    if (edgesActions.size() > 1) {
        QMenu* arcMenu = new QMenu;
        for (auto action : edgesActions) {
            arcMenu->addAction(action);
        }
        toolbar->addTool(arcMenu);
    } else {
        toolbar->addTool(edgesActions.takeFirst());
    }

    auto m_removeAction = makeAction(
        QIcon(":/images/tools/remove.svg"),
        tr("Remove"),
        true,
        A_Remove,
        m_actionGroup,
        QKeySequence::fromString(QString::fromStdString(std::format("Shift+{}", sequence++))),
        tr("Remove object/edge from net")
    );

    auto m_moveAction = makeAction(
        QIcon(":/images/tools/move.svg"),
        tr("Move"),
        true,
        A_Move,
        m_actionGroup,
        QKeySequence::fromString(QString::fromStdString(std::format("Shift+{}", sequence++))),
        tr("Move selected object(s)")
    );

    connect(m_actionGroup, &QActionGroup::triggered, this, [this]() {
        if (auto checked = m_actionGroup->checkedAction(); checked) {
            m_mode = (Mode)checked->data().toInt();
        } else {
            m_mode = A_Nothing;
        }
    });

    toolbar->addTool(m_removeAction);
    toolbar->addTool(m_moveAction);
}

void EditorToolController::setMode(Mode mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    edgeRollback();
}

bool EditorToolController::onMousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        switch (m_mode) {
            case A_Add:
                insertVertex(event);
                break;
            case A_Remove:
                removeObject(event);
                break;
            case A_Connection:
                if (m_currentConnection) {
                    edgeCommit(event);
                } else {
                    edgeStart(event);
                }
                break;
            case A_Move:
                QCoreApplication::sendEvent(m_scene, event);

                if (auto dragged = dynamic_cast<PetriObject*>(m_scene->mouseGrabberItem()); dragged) {
                    m_dragInProgress = true;
                    for (auto item : m_scene->selectedItems()) {
                        if (auto petriItem = dynamic_cast<PetriObject*>(item)) {
                            m_draggedItems.push_back({petriItem, petriItem->scenePos()});
                        }
                    }
                }
                break;
            case A_Rotation:
                //scene->rotateObject(event);
                break;
            case A_Marker:
                break;
            default:
                break;
        }
    } else if (event->button() == Qt::RightButton) {
        if (m_mode == A_Connection) {
            edgeRollback();
        }
        else {
            return false;
        }
    }
    return true;
}

bool EditorToolController::onMouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    switch (m_mode) {
        case A_Connection:
            if (m_currentConnection) {
                m_currentConnection->moveTo(event->scenePos());
                m_currentConnection->updatePosition();
            }
            break;
        case A_Remove:
        case A_Move:
            break;
        default:
            return true;
    }
    return false;
}

bool EditorToolController::onMouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event)
    if (m_dragInProgress) {
        m_dragInProgress = false;
        m_draggedItems.clear();
    }
    return false;
}

void EditorToolController::insertVertex(QGraphicsSceneMouseEvent *event)
{
    auto vertex = m_vertexFactory->create();
    if (vertex) {
        vertex->setPos(event->scenePos());
        m_scene->model()->addVertex(vertex);
    }
}

void EditorToolController::removeObject(QGraphicsSceneMouseEvent* event)
{
    auto item = m_scene->itemAt(event->scenePos(), QTransform());
    if (auto vertex = dynamic_cast<IVertex*>(item); vertex) {
        m_scene->model()->removeVertex(vertex);
    } else if (auto edge = dynamic_cast<IEdge*>(item); edge) {
        m_scene->model()->removeEdge(edge);
    }
}

void EditorToolController::edgeStart(QGraphicsSceneMouseEvent* event)
{
    auto item = m_scene->netItemAt(event->scenePos());
    if (item) {
        m_currentConnection = m_edgeFactory->create();
        if (!m_currentConnection->start(item)) {
            delete m_currentConnection;
            m_currentConnection = nullptr;
            return;
        }

        m_currentConnection->moveTo(event->scenePos());
        m_scene->addItem(m_currentConnection);
    }
}

void EditorToolController::edgeCommit(QGraphicsSceneMouseEvent* event)
{
    if (!m_currentConnection) {
        return;
    }

    if (auto item = m_scene->netItemAt(event->scenePos()); item) {
        if (!m_currentConnection->finish(item)) {
            edgeRollback();
            return;
        }

        m_scene->removeItem(m_currentConnection); // must delete before insert (TODO: maybe optimize it)
        if (!m_scene->model()->addEdge(m_currentConnection)) {
            delete m_currentConnection;
        }
        m_currentConnection = nullptr;
    }
}

void EditorToolController::edgeRollback()
{
    if (m_currentConnection) {
        m_scene->removeItem(m_currentConnection);

        delete m_currentConnection;
        m_currentConnection = nullptr;
    }
}

