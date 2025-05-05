// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef TOPOLOGYWIDGET_H
#define TOPOLOGYWIDGET_H

#include <QTableWidget>

class TopologyModel;
class IEdge;
class NetModel;
class IVertex;

class TopologyWidget : public QTableView {

public:
    explicit TopologyWidget(QWidget* parent = nullptr);

    void setNetModel(NetModel* model);

    NetModel* netModel() const { return m_model; }

    void reload();

public slots:

    void onSelectionChanged(const QItemSelection&, const QItemSelection&);
    void onModelSelectionChanged();

private:
    NetModel* m_model = nullptr;
    TopologyModel* m_topologyModel = nullptr;

    QHash<IEdge*, QTableWidgetItem*> m_edgesMap;
    QList<QMetaObject::Connection> m_objectConnections;

    bool m_blockAcceptModelSignals = false;
};

#endif // TOPOLOGYWIDGET_H
