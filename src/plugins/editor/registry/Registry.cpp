// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Registry.h"

#include "IEdge.h"
#include "IVertex.h"

Registry* Registry::REGISTRY = new Registry;

bool Registry::registerVertex(IVertex* factory)
{
    REGISTRY->m_vertexFactories.insert(factory->typeId(), factory);
    return true;
}

IVertex* Registry::vertex(const QByteArray& type)
{
    return REGISTRY->m_vertexFactories.find(type).value();
}

const QMap<QByteArray, IVertex*>& Registry::vertices() { return REGISTRY->m_vertexFactories; }

bool Registry::registerEdge(IEdge* factory)
{
    REGISTRY->m_edgeFactories.insert(factory->typeId(), factory);
    return true;
}

IEdge* Registry::edge(const QByteArray& type)
{
    return REGISTRY->m_edgeFactories.find(type).value();
}

const QMap<QByteArray, IEdge*>& Registry::edges() { return REGISTRY->m_edgeFactories; }
