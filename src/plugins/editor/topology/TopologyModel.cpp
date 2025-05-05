// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "TopologyModel.h"

#include "../registry/IEdge.h"
#include "../registry/IVertex.h"

int TopologyModel::rowCount(const QModelIndex&) const
{
    return (int)m_edges.count();
}

int TopologyModel::columnCount(const QModelIndex&) const { return 3; }

QVariant TopologyModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        auto edge = m_edges[index.row()];
        int col = index.column();
        if (col == 0) {
            return edge->typeId();
        }
        if (col == 1) {
            return QString("%1%2").arg(edge->begin()->symbol()).arg(edge->begin()->id());
        }
        if (col == 2) {
            return QString("%1%2").arg(edge->end()->symbol()).arg(edge->end()->id());
        }
    }

    return {};
}

QVariant TopologyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return tr("Type");
        }
        if (section == 1) {
            return tr("Begin");
        }
        if (section == 2) {
            return tr("End");
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

QModelIndex TopologyModel::index(IEdge* edge) const
{
    // if (row >= 0 && row < m_edges.size()) {
    //
    // }
    return QAbstractTableModel::index(m_edges.indexOf(edge), 2);
}

IEdge* TopologyModel::edgeAt(int idx) const
{
    if (idx < 0 || idx >= m_edges.size()) {
        return nullptr;
    }

    return m_edges[idx];
}

void TopologyModel::clear()
{
    beginResetModel();
    m_edges.clear();
    endResetModel();
}

void TopologyModel::onEdgeInserted(IEdge* edge)
{
    beginInsertRows(QModelIndex(), m_edges.size(), m_edges.size());
    m_edges.push_back(edge);
    endInsertRows();
}

void TopologyModel::onEdgeRemoved(IEdge* edge)
{
    int row = m_edges.indexOf(edge);
    if (row >= 0) {
        beginRemoveRows(QModelIndex(), row, row);
        m_edges.remove(row);
        endRemoveRows();
    }
}

void TopologyModel::onEdgeChanged(IEdge* edge)
{
    int row = m_edges.indexOf(edge);
    if (row >= 0) {
        emit dataChanged(createIndex(row, 0), createIndex(row, columnCount(QModelIndex()) - 1));
    }
}
