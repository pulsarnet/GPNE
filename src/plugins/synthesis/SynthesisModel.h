// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_SYNTHESIS_MODEL_H
#define GPNE_SYNTHESIS_MODEL_H

#include <QAbstractTableModel>
#include "SynthesisWorker.h"

namespace ptn::modules::decompose
{
    struct DecomposeContext;
}

class SynthesisModel : public QAbstractTableModel {

public:
    explicit SynthesisModel(QObject* parent = nullptr);

    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    void clearPrograms();
    void appendPrograms(const QList<Evaluated>& programs);
    const Evaluated& program(int row) const;

    void setAdditionalColumns(const QList<QString>& columns);

private:
    QList<Evaluated> m_programs;
    QList<QString> m_columns;
};

#endif // GPNE_SYNTHESIS_MODEL_H
