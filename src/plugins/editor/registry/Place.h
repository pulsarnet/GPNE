// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PLACE_H
#define PLACE_H

#include "IVertex.h"

class Place : public IPlace {

    Q_GADGET
    GPNE_GADGET_OVERRIDE
    Q_PROPERTY(usize markers READ markers WRITE setMarkers)

public:
    QByteArray typeId() const override { return "Place"; }

    ptn::net::vertex::VertexType vertexType() const override { return ptn::net::vertex::TPlace; }

    char symbol() const noexcept override { return 'P'; }

    QIcon icon() const override { return QIcon(":/images/tools/place.svg"); }

    virtual ~Place();

    IVertex* create() const override;
    IVertex* fromJson(const QJsonObject& object) const override;
    QJsonObject toJson() const override;

    // graphics item
    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QPointF jointPoint(const QPointF&) const override;
    QPointF center() const override;

    void submitData() override;
    void restoreData() override;
    void removeData() override;

    QList<IVertex*> getItemsFromNet(ptn::net::PetriNet* m_net) override;

    // custom
    usize markers() const { return m_markers; }
    void setMarkers(usize markers);

private:
    usize m_markers = 0;
};

#endif // PLACE_H
