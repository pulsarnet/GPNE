// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "MatrixWindow.h"
#include "NamedMatrixModel.h"
#include <editor/model/NetModel.h>
#include <editor/registry/IEdge.h>
#include <editor/registry/IVertex.h>
#include <editor/registry/Registry.h>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableView>

QString headerName(IVertex* vertex)
{
    return QString("%1%2").arg(vertex->symbol()).arg(vertex->id());
}

MatrixWindow::MatrixWindow(NetModel* model, QWidget* parent)
    : QDialog(parent)
    , m_layout(new QGridLayout(this))
{
    setWindowTitle("I/O Matrix View");
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setEnabled(true);
    setModal(false);

    const QList directions{IEdge::PlaceToTransition, IEdge::TransitionToPlace};
    int row = 0;
    for (auto edge : Registry::edges()) {
        int column = 0;
        for (const auto direction : directions) {
            if (!edge->allowDirection(direction)) {
                continue;
            }

            auto view = new QTableView;
            view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
            view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

            auto matrixModel = new NamedMatrixModel<i32>();
            view->setModel(matrixModel);
            view->resizeColumnsToContents();
            view->resizeRowsToContents();

            m_matrixModels.insert({edge->typeId(), direction}, matrixModel);

            QString layoutName =
                QString("%1 (%2)")
                    .arg(edge->typeId())
                    .arg(direction == IEdge::PlaceToTransition ? "input" : "output");

            m_layout->addWidget(new QLabel(layoutName), row, column);
            m_layout->addWidget(view, row + 1, column);
            column += 1;
        }
        row += 2;
    }

    setModel(model);
}

void MatrixWindow::closeEvent(QCloseEvent* event)
{
    emit onWindowClose(this);
    QWidget::closeEvent(event);
}

void MatrixWindow::setModel(NetModel* model)
{
    if (m_model == model) {
        return;
    }

    if (m_model) {
        clear();
    }

    m_model = model;
    if (m_model) {
        m_objectConnections = {
            connect(m_model, &NetModel::vertexInserted, this, &MatrixWindow::onVertexInserted),
            connect(m_model, &NetModel::vertexRemoved, this, &MatrixWindow::onVertexRemoved),

            connect(m_model, &NetModel::edgeInserted, this, &MatrixWindow::onEdgeChanged),
            connect(m_model, &NetModel::edgeChanged, this, &MatrixWindow::onEdgeChanged),
            connect(m_model, &NetModel::edgeRemoved, this, &MatrixWindow::onEdgeRemoved),
        };

        for (auto vertex : m_model->vertices()) {
            onVertexInserted(vertex);
        }

        for (auto edge : m_model->edges()) {
            onEdgeChanged(edge);
        }
    }
}

void MatrixWindow::clear()
{
    m_places.clear();
    m_transitions.clear();

    for (auto matrixModel : m_matrixModels.values()) {
        matrixModel->clear();
    }

    for (const QMetaObject::Connection& connection : m_objectConnections) {
        disconnect(connection);
    }
}

void MatrixWindow::onVertexInserted(IVertex* vertex)
{
    if (vertex->vertexType() == ptn::net::vertex::TPlace) {
        m_places.append(vertex);
        onRowAdded(headerName(vertex));
    } else if (vertex->vertexType() == ptn::net::vertex::TTransition) {
        m_transitions.append(vertex);
        onColumnAdded(headerName(vertex));
    }
}

void MatrixWindow::onVertexRemoved(IVertex* vertex)
{
    if (vertex->vertexType() == ptn::net::vertex::TPlace) {
        auto it = std::find(m_places.begin(), m_places.end(), vertex);
        m_places.remove(std::distance(m_places.begin(), it));
        onRowRemoved(headerName(vertex));
    } else if (vertex->vertexType() == ptn::net::vertex::TTransition) {
        auto it = std::find(m_transitions.begin(), m_transitions.end(), vertex);
        m_transitions.remove(std::distance(m_transitions.begin(), it));
        onColumnRemoved(headerName(vertex));
    }
}

void MatrixWindow::onEdgeChanged(IEdge* edge)
{
    IVertex* place = edge->place();
    IVertex* transition = edge->transition();

    const int placeIndex = m_places.indexOf(place);
    const int transitionIndex = m_transitions.indexOf(transition);

    EdgeKey pTot = {edge->typeId(), IEdge::PlaceToTransition};
    EdgeKey tTop = {edge->typeId(), IEdge::TransitionToPlace};

    if (edge->allowDirection(IEdge::PlaceToTransition)) {
        m_matrixModels[pTot]
            ->setValue(placeIndex, transitionIndex, edge->weight(IEdge::PlaceToTransition));
    }

    if (edge->allowDirection(IEdge::TransitionToPlace)) {
        m_matrixModels[tTop]
            ->setValue(placeIndex, transitionIndex, edge->weight(IEdge::TransitionToPlace));
    }
}

void MatrixWindow::onEdgeRemoved(IEdge* edge)
{
    IVertex* place = edge->place();
    IVertex* transition = edge->transition();

    const int placeIndex = m_places.indexOf(place);
    const int transitionIndex = m_transitions.indexOf(transition);

    EdgeKey pTot = {edge->typeId(), IEdge::PlaceToTransition};
    EdgeKey tTot = {edge->typeId(), IEdge::TransitionToPlace};

    if (edge->allowDirection(IEdge::PlaceToTransition)) {
        m_matrixModels[pTot]->setValue(placeIndex, transitionIndex, 0);
    }

    if (edge->allowDirection(IEdge::TransitionToPlace)) {
        m_matrixModels[tTot]->setValue(placeIndex, transitionIndex, 0);
    }
}

void MatrixWindow::onRowAdded(QString name)
{
    for (auto model : m_matrixModels.values()) {
        model->addRow(name);
    }
}

void MatrixWindow::onRowRemoved(QString name)
{
    for (auto model : m_matrixModels.values()) {
        model->removeRow(name);
    }
}

void MatrixWindow::onColumnAdded(QString name)
{
    for (auto model : m_matrixModels.values()) {
        model->addColumn(name);
    }
}

void MatrixWindow::onColumnRemoved(QString name)
{
    for (auto model : m_matrixModels.values()) {
        model->removeColumn(name);
    }
}
