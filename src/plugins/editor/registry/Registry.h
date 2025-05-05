// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef REGISTRY_H
#define REGISTRY_H

#include <QMap>

class IVertex;
class IEdge;

class Registry {

public:
    static bool registerVertex(IVertex*);
    static IVertex* vertex(const QByteArray&);
    static const QMap<QByteArray, IVertex*>& vertices();

    static bool registerEdge(IEdge*);
    static IEdge* edge(const QByteArray&);
    static const QMap<QByteArray, IEdge*>& edges();

private:
    QMap<QByteArray, IVertex*> m_vertexFactories;
    QMap<QByteArray, IEdge*> m_edgeFactories;

    static Registry* REGISTRY;
};

#endif // REGISTRY_H
