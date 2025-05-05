// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "TopologyWidget.h"

#include <QHeaderView>

#include "../model/NetModel.h"
#include "../registry/IEdge.h"
#include "../registry/IVertex.h"
#include "TopologyModel.h"

TopologyWidget::TopologyWidget(QWidget* parent)
    : QTableView(parent)
    , m_topologyModel(new TopologyModel)
{
    setSelectionMode(SelectionMode::ExtendedSelection);
    setSelectionBehavior(SelectionBehavior::SelectRows);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setHidden(true);

    setModel(m_topologyModel);

    connect(
        selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &TopologyWidget::onSelectionChanged
    );
}

void TopologyWidget::setNetModel(NetModel* model)
{
    if (m_model == model) {
        return;
    }

    if (m_model) {
        for (const QMetaObject::Connection& connection : m_objectConnections) {
            disconnect(connection);
        }
    }

    m_model = model;
    reload();
    if (m_model) {
        m_objectConnections = {
            connect(
                m_model,
                &NetModel::edgeInserted,
                m_topologyModel,
                &TopologyModel::onEdgeInserted
            ),
            connect(
                m_model,
                &NetModel::edgeRemoved,
                m_topologyModel,
                &TopologyModel::onEdgeRemoved
            ),
            connect(
                m_model,
                &NetModel::edgeChanged,
                m_topologyModel,
                &TopologyModel::onEdgeChanged
            ),

            connect(
                m_model,
                &NetModel::selectionChanged,
                this,
                &TopologyWidget::onModelSelectionChanged
            ),
        };
    }
}

void TopologyWidget::reload()
{
    m_topologyModel->clear();
    if (m_model) {
        for (auto edge : m_model->edges()) {
            m_topologyModel->onEdgeInserted(edge);
        }
    }
}

void TopologyWidget::onSelectionChanged(const QItemSelection&, const QItemSelection&)
{
    if (m_blockAcceptModelSignals) {
        return;
    }

    QList<IEdge*> selected;
    auto rows = selectionModel()->selectedRows();
    for (auto idx : rows) {
        auto edge = m_topologyModel->edgeAt(idx.row());
        if (edge) {
            selected.append(edge);
        }
    }
    m_blockAcceptModelSignals = true;
    m_model->setSelectedEdges(selected);
    m_blockAcceptModelSignals = false;
}

void TopologyWidget::onModelSelectionChanged()
{
    clearSelection();
    for (auto edge : m_model->selectedEdges()) {
        selectionModel()->select(
            m_topologyModel->index(edge),
            QItemSelectionModel::Select | QItemSelectionModel::Rows
        );
    }
}
