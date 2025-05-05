// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef IVERTEX_H
#define IVERTEX_H

#include <editor/elements/PetriObject.h>
#include <editor/Macros.h>
#include <editor/model/NetModel.h>
#include "IItem.h"
#include <ptn/net.h>
#include <QIcon>
#include <QObject>

class IEdge;

class IVertex : public IItem, public PetriObject {

    Q_GADGET
    GPNE_GADGET
    Q_PROPERTY(QPointF position READ pos WRITE setPos)

public:
    enum VertexClass
    {
        Place,
        Transition
    };

    virtual QByteArray typeId() const = 0;
    virtual VertexClass classId() const = 0;
    virtual ptn::net::vertex::VertexType vertexType() const = 0; // MUST BE UNIQUE
    virtual QIcon icon() const = 0;

    usize id() const { return m_id; }
    void setId(usize id);

    usize displayId() const final { return id(); }

    virtual IVertex* create() const = 0;

    // json
    virtual IVertex* fromJson(const QJsonObject& object) const;
    virtual QJsonObject toJson() const;

    IVertex* asIVertex() { return this; }
    QGraphicsItem* asGraphicsItem() { return this; }

    void addEdge(IEdge*);
    void removeEdge(IEdge*);

    const QSet<IEdge*>& edges() const { return m_edges; }

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) final;

    // model
    virtual void emitDataChanged();

    // restore
    virtual QList<IVertex*> getItemsFromNet(ptn::net::PetriNet* m_net) = 0;

    ~IVertex() override;

    friend class NetModel;

protected:
    void updateEdges();

private:
    QSet<IEdge*> m_edges;
    usize m_id = 0;
};

Q_DECLARE_METATYPE(IVertex*)

class IPlace : public IVertex {
public:
    VertexClass classId() const final { return Place; }
};

class ITransition : public IVertex {
public:
    VertexClass classId() const final { return Transition; }
};

#endif // IVERTEX_H
