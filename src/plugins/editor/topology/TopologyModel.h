// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef TOPOLOGYMODEL_H
#define TOPOLOGYMODEL_H

#include <QAbstractTableModel>

class IEdge;

class TopologyModel : public QAbstractTableModel {
public:
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    ~TopologyModel() override = default;

    QModelIndex index(IEdge*) const;
    IEdge* edgeAt(int idx) const;
    void clear();

public slots:
    void onEdgeInserted(IEdge*);
    void onEdgeRemoved(IEdge*);
    void onEdgeChanged(IEdge*);

private:
    QList<IEdge*> m_edges;
};

#endif // TOPOLOGYMODEL_H
