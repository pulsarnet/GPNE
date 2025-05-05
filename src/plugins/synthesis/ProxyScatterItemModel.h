// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROXYSCATTERITEMMODEL_H
#define PROXYSCATTERITEMMODEL_H

#include <QAbstractProxyModel>

class ProxyScatterItemModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    ProxyScatterItemModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

public slots:

    void onSourceModelChanged();
    void updateRoleNames();

signals:

    void roleNamesChanged();

private:

    QList<QMetaObject::Connection> m_connections;
    QHash<int, QByteArray> m_roleNames;
};

#endif //PROXYSCATTERITEMMODEL_H
