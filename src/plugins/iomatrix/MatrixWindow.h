// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_MATRIXWINDOW_H
#define GPNE_MATRIXWINDOW_H

#include "NamedMatrixModel.h"
#include <editor/registry/IEdge.h>
#include <QDialog>

class QGridLayout;

typedef std::pair<QByteArray, IEdge::Direction> EdgeKey;

class MatrixWindow : public QDialog {

    Q_OBJECT

public:
    explicit MatrixWindow(NetModel* model, QWidget* parent = nullptr);

    void closeEvent(QCloseEvent* event) override;

    void setModel(NetModel* model);

    void clear();

signals:

    void onWindowClose(QWidget*);

public slots:

    void onVertexInserted(IVertex*);
    void onVertexRemoved(IVertex*);

    void onEdgeChanged(IEdge*);
    void onEdgeRemoved(IEdge*);

protected:
    void onRowAdded(QString name);
    void onRowRemoved(QString name);

    void onColumnAdded(QString name);
    void onColumnRemoved(QString name);

private:
    QGridLayout* m_layout;

    QMap<EdgeKey, NamedMatrixModel<i32>*> m_matrixModels;
    QList<IVertex*> m_places;
    QList<IVertex*> m_transitions;

    NetModel* m_model{nullptr};
    QList<QMetaObject::Connection> m_objectConnections;
};

#endif // GPNE_MATRIXWINDOW_H
