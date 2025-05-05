// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "JsonFormat.h"
#include "../registry/IEdge.h"
#include "../registry/IVertex.h"
#include "../registry/Registry.h"
#include "NetModel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

typedef std::pair<QByteArray, usize> VertexKey;

std::optional<VertexKey> parseVertexKey(const QJsonObject&, const char*);

bool JsonFormat::load(NetModel* model, const QJsonObject& object)
{
    QHash<VertexKey, IVertex*> verticesToAdd;
    QList<IEdge*> edgesToAdd;

    const QJsonValue verticesJson = object["vertices"];
    const QJsonValue edgesJson = object["edges"];

    if (verticesJson.isUndefined()) {
        model->clear();
        return true;
    }
    if (!verticesJson.isObject()) {
        qWarning() << "Expecting object";
        return false;
    }

    if (!edgesJson.isUndefined() && !edgesJson.isObject()) {
        qWarning() << "Expecting object";
        return false;
    }

    const QJsonObject vertices = verticesJson.toObject();
    const QJsonObject edges = edgesJson.isUndefined() ? QJsonObject() : edgesJson.toObject();

    for (auto [_hide, vertex] : Registry::vertices().asKeyValueRange()) {
        const QJsonValue vertexValue = vertices[QLatin1String(vertex->typeId())];
        if (!vertexValue.isArray()) {
            qWarning() << "Expecting array '" << vertex->typeId() << "'";
            continue;
        }

        const QJsonArray vertexArray = vertexValue.toArray();
        for (const QJsonValue vertexJsonElement : vertexArray) {
            if (!vertexJsonElement.isObject()) {
                qWarning() << "Expecting object for vertex element";
                continue;
            }

            if (auto element = vertex->fromJson(vertexJsonElement.toObject()); element) {
                verticesToAdd.insert({element->typeId(), element->id()}, element);
            } else {
                qWarning() << std::format(
                    "Cannot load vertex with type '{}' from json",
                    QString(vertex->typeId()).toStdString()
                );
            }
        }
    }

    for (auto [_hide, edge] : Registry::edges().asKeyValueRange()) {
        const QJsonValue edgeValue = edges[QLatin1String(edge->typeId())];
        if (edgeValue.isUndefined()) {
            continue;
        }

        if (!edgeValue.isArray()) {
            qWarning() << "Expecting array '" << edge->typeId() << "'";
            continue;
        }

        const QJsonArray edgeArray = edgeValue.toArray();
        for (const QJsonValue edgeJsonElement : edgeArray) {
            if (!edgeJsonElement.isObject()) {
                qWarning() << "Expecting object for edge element";
                continue;
            }

            const QJsonObject edgeJsonObject = edgeJsonElement.toObject();
            std::optional<VertexKey> placeOpt = parseVertexKey(edgeJsonObject, "place");
            std::optional<VertexKey> transitionOpt = parseVertexKey(edgeJsonObject, "transition");
            if (!placeOpt.has_value() || !transitionOpt.has_value()) {
                continue;
            }

            VertexKey placeKey = placeOpt.value();
            VertexKey transitionKey = transitionOpt.value();

            const QJsonValue edgeData = edgeJsonObject["data"];
            if (edgeData.isUndefined() || !edgeData.isObject()) {
                qWarning() << "'data' field has invalid type: expecting object";
                continue;
            }

            const QJsonObject data = edgeData.toObject();
            const QJsonValue directions = data["directions"];
            if (directions.isUndefined() || !directions.isDouble()) {
                qWarning() << "'directions' field has invalid type: expecting integer";
                continue;
            }

            auto placeIt = verticesToAdd.find(placeKey);
            auto transitionIt = verticesToAdd.find(transitionKey);

            if (placeIt == verticesToAdd.end() || transitionIt == verticesToAdd.end()) {
                qWarning() << "vertices for edge not found";
                continue;
            }

            IEdge::Directions dir = IEdge::Directions::fromInt(directions.toInt());
            IVertex* place = placeIt.value();
            IVertex* transition = transitionIt.value();

            if (place->vertexType() != ptn::net::vertex::TPlace ||
                transition->vertexType() != ptn::net::vertex::TTransition)
            {
                qWarning() << "invalid vertex type";
                continue;
            }

            IVertex* start{nullptr};
            IVertex* finish{nullptr};
            if (dir.testFlag(IEdge::Direction::PlaceToTransition)) {
                start = place;
                finish = transition;
            } else {
                start = transition;
                finish = place;
            }

            IEdge* newEdge = edge->create();
            if (!newEdge->start(start)) {
                qWarning() << "invalid start vertex";
                delete newEdge;
                continue;
            }

            if (!newEdge->finish(finish)) {
                qWarning() << "invalid finish vertex";
                delete newEdge;
                continue;
            }

            if (!newEdge->loadFromJson(edgeData.toObject())) {
                qWarning() << "cannot load data from json";
                delete newEdge;
                continue;
            }

            edgesToAdd.append(newEdge);
        }
    }

    model->clear();

    for (auto vertex : verticesToAdd) {
        model->addVertex(vertex);
    }

    for (auto edge : edgesToAdd) {
        model->addEdge(edge);
    }

    return true;
}

void JsonFormat::write(NetModel* model, QJsonObject& object)
{
    QMap<QByteArray, QJsonArray> vertexMap;
    QMap<QByteArray, QJsonArray> edgeMap;

    for (auto vertex : model->vertices().values()) {
        QJsonObject vertexObject = vertex->toJson();
        vertexMap[vertex->typeId()].append(vertexObject);
    }

    for (auto edge : model->edges().values()) {
        QJsonObject edgeObject = edge->toJson();
        edgeMap[edge->typeId()].append(edgeObject);
    }

    QJsonObject vertices;
    for (auto [k, v] : vertexMap.asKeyValueRange()) {
        vertices.insert(QLatin1String(k), v);
    }

    QJsonObject edges;
    for (auto [k, v] : edgeMap.asKeyValueRange()) {
        edges.insert(QLatin1String(k), v);
    }

    object["vertices"] = vertices;
    object["edges"] = edges;
}

std::optional<VertexKey> parseVertexKey(const QJsonObject& object, const char* name)
{
    const QJsonValue value = object[name];
    if (value.isUndefined()) {
        qWarning() << std::format("'{}' field not found", name);
        return std::nullopt;
    }

    if (!value.isObject()) {
        qWarning() << std::format("'{}' field has invalid type: expecting json object", name);
        return std::nullopt;
    }

    const QJsonObject vertex = value.toObject();
    const QJsonValue id = value["id"];
    const QJsonValue type = value["type"];

    if (id.isUndefined()) {
        qWarning() << "'id' field not found";
        return std::nullopt;
    }

    if (!id.isDouble()) {
        qWarning() << "'id' field has invalid type: expecting integer";
        return std::nullopt;
    }

    if (type.isUndefined()) {
        qWarning() << "'type' field not found";
        return std::nullopt;
    }

    if (!type.isString()) {
        qWarning() << "'type' field has invalid type: expecting string";
        return std::nullopt;
    }

    return std::pair{type.toString().toUtf8(), id.toInteger()};
}
