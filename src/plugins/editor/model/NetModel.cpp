// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "NetModel.h"

#include "../registry/IEdge.h"
#include "../registry/IVertex.h"
#include "../registry/Registry.h"

NetModel::NetModel() : QObject(nullptr)
{
    m_net = ptn::net::PetriNet::create();
    init();
}

NetModel::NetModel(ptn::net::PetriNet* net) : QObject(nullptr), m_net(net)
{
    init();
}

/**
 * @brief Add vertex to model
 *
 * @param vertex - vertex to be inserted
 * @return true if vertex inserted, otherwise false if vertex already exists
 */
bool NetModel::addVertex(IVertex* vertex)
{
    // If vertex already exists in model return true
    if (vertex->m_model == this) {
        qWarning() << "Vertex already exists in current model";
        return true;
    }

    // If vertex already exists in another model remove it from there
    if (vertex->m_model) {
        vertex->m_model->removeVertex(vertex);
    }

    vertex->m_model = this;
    vertex->submitData();

    m_vertices.insert(vertex);
    emit vertexInserted(vertex);
    return true;
}

/**
 * Remove vertex from model
 *
 * Also removes edges which connect this vertex
 *
 * @param vertex  - vertex to be removed
 * @return true if vertex removed, otherwise return false if vertex not exists
 */
bool NetModel::removeVertex(IVertex* vertex)
{
    if (vertex->m_model != this) {
        qDebug() << "Vertex not found";
        return false;
    }

    auto vertexEdge = vertex->edges();
    for (auto edge : vertexEdge) {
        removeEdge(edge);
        delete edge;
    }

    vertex->removeData();
    vertex->m_model = nullptr;

    m_vertices.remove(vertex);
    emit vertexRemoved(vertex);
    return true;
}

void NetModel::setSelectedVertices(QList<IVertex*> vertices)
{
    m_selectedVertices = vertices;
    emit selectionChanged();
}

bool NetModel::addEdge(IEdge* edge)
{
    auto it = std::find_if(m_edges.begin(), m_edges.end(), [=](IEdge* _edge) {
        return (_edge->begin() == edge->begin() && _edge->end() == edge->end()) ||
               (_edge->end() == edge->begin() && _edge->begin() == edge->end());
    });

    if (it == m_edges.end()) {
        edge->m_model = this;
        edge->submitData();

        m_edges.insert(edge);
        emit edgeInserted(edge);
    } else if (edge->typeId() == (*it)->typeId() && (*it)->end() == edge->begin() && (*it)->begin() == edge->end()) {
        (*it)->setDirections(IEdge::PlaceToTransition | IEdge::TransitionToPlace);
        delete edge;
    } else {
        qDebug() << "Edge already exists";
        return false;
    }
    return true;
}

bool NetModel::removeEdge(IEdge* edge)
{
    edge->removeData();
    edge->m_model = nullptr;

    if (m_edges.remove(edge)) {
        emit edgeRemoved(edge);
        return true;
    }
    return false;
}

const QSet<IVertex*>& NetModel::vertices() const
{
    return m_vertices;
}

const QSet<IEdge*>& NetModel::edges() const
{
    return m_edges;
}

const QList<IVertex*>& NetModel::selectedVertices() const
{
    return m_selectedVertices;
}

const QList<IEdge*>& NetModel::selectedEdges() const
{
    return m_selectedEdges;
}

ptn::net::PetriNet* NetModel::net() const
{
    return m_net;
}

void NetModel::setSelectedEdges(QList<IEdge*> edges)
{
    m_selectedEdges = edges;
    emit selectionChanged();
}

void NetModel::clear()
{
    setSelectedVertices({});
    setSelectedEdges({});

    while (!m_vertices.empty()) {
        auto vertex = *m_vertices.begin();
        removeVertex(vertex);
        delete vertex;
    }

    m_vertices.clear();
    m_edges.clear();

    m_net->clear();
}

NetModel* NetModel::copy() const {
    NetModel* model = new NetModel(ptn::net::PetriNet::clone(m_net));
    QHash<vertex_id, IVertex*> vertexMap;
    for (auto vertex : m_vertices) {
        vertexMap.insert({ vertex->vertexType(), vertex->id()}, vertex);
    }

    for (auto vertex : model->vertices()) {
        auto v = vertexMap.value({ vertex->vertexType(), vertex->id()});
        vertex->setPos(v->pos());
    }
    return model;
}

void NetModel::init() {
    for (auto vertex : Registry::vertices()) {
        auto items = vertex->getItemsFromNet(m_net);
        for (auto item : items) {
            item->m_model = this;
            item->restoreData();

            m_vertices.insert(item);
            emit vertexInserted(item);
        }
    }

    QHash<edge_type, IEdge*> edges;
    for (auto factory : Registry::edges()) {
        auto items = factory->getEdgePairs(m_net);
        for (auto item : items) {
            auto rev_item = std::make_pair(item.second, item.first);
            if (edges.contains(rev_item)) {
                edges[rev_item]->setDirections(IEdge::PlaceToTransition | IEdge::TransitionToPlace);
                continue;
            }

            auto [begin_t, begin_id] = item.first;
            auto [end_t, end_id] = item.second;

            IVertex* begin = nullptr;
            IVertex* end = nullptr;

            for (auto vertex : m_vertices) {
                if (vertex->vertexType() == begin_t && vertex->id() == begin_id) {
                    begin = vertex;
                } else if (vertex->vertexType() == end_t && vertex->id() == end_id) {
                    end = vertex;
                }
            }

            if (!begin || !end) {
                qWarning() << "Vertices not found";
                continue;
            }

            auto edge = factory->create();
            edge->start(begin);
            edge->finish(end);
            edges.insert(item, edge);
        }
    }

    for (auto edge : edges) {
        edge->m_model = this;
        edge->restoreData();
        m_edges.insert(edge);
    }
}

NetModel::~NetModel() { clear(); }
