// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ProxyScatterItemModel.h"

ProxyScatterItemModel::ProxyScatterItemModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
    connect(this, &QAbstractProxyModel::sourceModelChanged, this, &ProxyScatterItemModel::onSourceModelChanged);
}

QHash<int, QByteArray> ProxyScatterItemModel::roleNames() const {
    return m_roleNames;
}

QModelIndex ProxyScatterItemModel::index(int row, int column, const QModelIndex &parent) const {
    Q_UNUSED(parent);
    if (sourceModel()) {
        return createIndex(row, column);
    }
    return QModelIndex();
}

QModelIndex ProxyScatterItemModel::parent(const QModelIndex &child) const {
    if (sourceModel()) {
        return sourceModel()->parent(child);
    }
    return QModelIndex();
}

int ProxyScatterItemModel::rowCount(const QModelIndex &parent) const {
    if (sourceModel()) {
        return sourceModel()->rowCount(parent);
    }
    return 0;
}

int ProxyScatterItemModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    if (sourceModel()) {
        return 1;
    }
    return 0;
}

QVariant ProxyScatterItemModel::data(const QModelIndex &index, int role) const {
    QVariant value = QAbstractProxyModel::data(index, role);
    if (role < Qt::UserRole) {
        value = QAbstractProxyModel::data(index, role);
    } else {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = sourceModel()->index(index.row(), columnIdx);
        value = sourceModel()->data(modelIndex, Qt::DisplayRole);
    }
    return value;
}

QModelIndex ProxyScatterItemModel::mapToSource(const QModelIndex &proxyIndex) const {
    return proxyIndex;
}

QModelIndex ProxyScatterItemModel::mapFromSource(const QModelIndex &sourceIndex) const {
    return sourceIndex;
}

void ProxyScatterItemModel::onSourceModelChanged() {
    for (auto conn : m_connections) {
        QObject::disconnect(conn);
    }

    if (sourceModel()) {
        m_connections = {
            connect(sourceModel(), &QAbstractItemModel::columnsInserted, this, &ProxyScatterItemModel::updateRoleNames),
            connect(sourceModel(), &QAbstractItemModel::columnsRemoved, this, &ProxyScatterItemModel::updateRoleNames),
            connect(sourceModel(), &QAbstractItemModel::modelReset, this, &ProxyScatterItemModel::updateRoleNames),
            connect(sourceModel(), &QAbstractItemModel::dataChanged, this, &QAbstractProxyModel::dataChanged),
            connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &QAbstractProxyModel::rowsInserted),
            connect(sourceModel(), &QAbstractItemModel::rowsRemoved, this, &QAbstractProxyModel::rowsRemoved),
        };
    }
}

void ProxyScatterItemModel::updateRoleNames() {
    QHash<int, QByteArray> roles;
    if (auto s = sourceModel()) {
        for (int i = 0; i < s->columnCount(QModelIndex()); i ++) {
            roles.insert(Qt::UserRole + i + 1, headerData(i, Qt::Horizontal).toByteArray());
        }
    }

    if (m_roleNames != roles) {
        m_roleNames = roles;
        emit roleNamesChanged();
    }
}
