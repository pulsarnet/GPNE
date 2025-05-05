// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GRAPHICSCENENETMODEL_H
#define GRAPHICSCENENETMODEL_H

#include <QObject>
#include <QSet>

class IEdge;
class IVertex;
namespace ptn::net
{
    struct PetriNet;
}

class NetModel : public QObject {

    Q_OBJECT

public:
    NetModel();
    explicit NetModel(ptn::net::PetriNet* net);
    ~NetModel() override;

    virtual bool addVertex(IVertex*);
    virtual bool removeVertex(IVertex*);

    virtual bool addEdge(IEdge*);
    virtual bool removeEdge(IEdge*);

    virtual const QSet<IVertex*>& vertices() const;
    virtual const QSet<IEdge*>& edges() const;

    virtual const QList<IVertex*>& selectedVertices() const;
    virtual const QList<IEdge*>& selectedEdges() const;

    virtual void setSelectedVertices(QList<IVertex*>);
    virtual void setSelectedEdges(QList<IEdge*>);

    ptn::net::PetriNet* net() const;

    void clear();

    NetModel* copy() const;

signals:

    void vertexInserted(IVertex*);
    void vertexRemoved(IVertex*);
    void vertexChanged(IVertex*);

    void edgeInserted(IEdge*);
    void edgeRemoved(IEdge*);
    void edgeChanged(IEdge*);

    void selectionChanged();

protected:

    void init();

private:
    ptn::net::PetriNet* m_net;

    QSet<IVertex*> m_vertices;
    QSet<IEdge*> m_edges;

    QList<IVertex*> m_selectedVertices;
    QList<IEdge*> m_selectedEdges;
};

#endif // GRAPHICSCENENETMODEL_H
