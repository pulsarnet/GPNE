// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef TRANSITION_H
#define TRANSITION_H

#include "IVertex.h"

class Transition : public ITransition {

    Q_GADGET
    GPNE_GADGET_OVERRIDE
    Q_PROPERTY(bool horizontal READ rotated WRITE setRotated)

public:
    Transition();
    virtual ~Transition();

    QByteArray typeId() const override { return "Transition"; }

    ptn::net::vertex::VertexType vertexType() const override
    {
        return ptn::net::vertex::TTransition;
    }

    char symbol() const noexcept override { return 'T'; }

    QIcon icon() const override { return QIcon(":/images/tools/transition.svg"); }

    IVertex* create() const override;
    // json
    IVertex* fromJson(const QJsonObject& object) const override;
    QJsonObject toJson() const override;

    // graphics item
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QPointF jointPoint(const QPointF&) const override;
    QPointF center() const override;

    void submitData() override;
    void restoreData() override;
    void removeData() override;

    QList<IVertex*> getItemsFromNet(ptn::net::PetriNet* m_net) override;

    // custom
    bool rotated() const { return m_rotated; }

    void setRotated(bool rotated);

private:
    void updateRect();

    QRectF m_rect;
    bool m_rotated = false;
};

#endif // TRANSITION_H
