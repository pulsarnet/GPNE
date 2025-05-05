// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SynthesisModel.h"
#include <utility>

SynthesisModel::SynthesisModel(QObject* parent) : QAbstractTableModel(parent) {}

int SynthesisModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_programs.size());
}

int SynthesisModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_columns.size() + 1;
}

QVariant SynthesisModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section > m_columns.size()) {
        return {};
    }

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return tr("Index");
        }
        return m_columns[section - 1];
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant SynthesisModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return QVariant::fromValue(m_programs[index.row()].idx);
        }
        return m_programs[index.row()].metrics[index.column() - 1];
    }

    return {};
}

void SynthesisModel::clearPrograms()
{
    beginResetModel();
    m_programs.clear();
    endResetModel();
}

void SynthesisModel::appendPrograms(const QList<Evaluated>& programs)
{
    beginInsertRows(QModelIndex(), m_programs.size(), m_programs.size() + programs.size() - 1);
    m_programs.append(programs);
    endInsertRows();
}

const Evaluated& SynthesisModel::program(int row) const {
    if (row < 0 || row >= m_programs.size()) {
        throw std::out_of_range("SynthesisModel::program");
    }

    return m_programs[row];
}

void SynthesisModel::setAdditionalColumns(const QList<QString>& columns)
{
    beginResetModel();
    m_columns = columns;
    endResetModel();
}
