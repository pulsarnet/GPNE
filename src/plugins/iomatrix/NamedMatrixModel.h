// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_NAMED_MATRIX_MODEL_H
#define GPNE_NAMED_MATRIX_MODEL_H

#include <editor/registry/IVertex.h>
#include <ptn/matrix.h>
#include <QAbstractTableModel>

typedef std::pair<QByteArray, usize> VertexKey;

template <typename T>
class NamedMatrixModel : public QAbstractTableModel {

public:
    explicit NamedMatrixModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {}

    bool dropMimeData(
        const QMimeData* data,
        Qt::DropAction action,
        int row,
        int column,
        const QModelIndex& parent
    ) override
    {
        Q_UNUSED(data)
        Q_UNUSED(action)
        Q_UNUSED(row)
        Q_UNUSED(column)
        Q_UNUSED(parent)
        return false;
    }

    [[nodiscard]]
    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        Q_UNUSED(index)
        return Qt::ItemFlag::ItemIsEnabled;
    }

    [[nodiscard]]
    int rowCount(const QModelIndex& parent) const override
    {
        Q_UNUSED(parent)
        return (int)m_matrix.nrows();
    }

    [[nodiscard]]
    int columnCount(const QModelIndex& parent) const override
    {
        Q_UNUSED(parent)
        return (int)m_matrix.ncols();
    }

    [[nodiscard]]
    QVariant data(const QModelIndex& index, int role) const override
    {
        if (role == Qt::DisplayRole) {
            return m_matrix[{(size_t)index.row(), (size_t)index.column()}];
        }

        return {};
    }

    [[nodiscard]]
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        Q_UNUSED(section)
        Q_UNUSED(orientation)
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Orientation::Horizontal) {
                return m_columns[section];
            }
            return m_rows[section];
        }

        return {};
    }

    [[nodiscard]]
    QModelIndex index(int row, int column, const QModelIndex& parent) const override
    {
        Q_UNUSED(parent);
        return createIndex(row, column);
    }

    [[nodiscard]]
    QModelIndex sibling(int row, int column, const QModelIndex& idx) const override
    {
        Q_UNUSED(idx);
        return createIndex(row, column);
    }

    void addRow(QString name)
    {
        beginInsertRows(QModelIndex(), m_rows.size(), m_rows.size());
        m_matrix.add_row();
        m_rows.append(name);
        endInsertRows();
    }

    void removeRow(QString name)
    {
        size_t pos = m_rows.indexOf(name);
        beginRemoveRows(QModelIndex(), pos, pos);
        m_matrix.remove_row(pos);
        m_rows.remove(pos);
        endRemoveRows();
    }

    void addColumn(QString name)
    {
        beginInsertColumns(QModelIndex(), m_columns.size(), m_columns.size());
        m_matrix.add_column();
        m_columns.append(name);
        endInsertColumns();
    }

    void removeColumn(QString name)
    {
        size_t pos = m_columns.indexOf(name);
        beginRemoveColumns(QModelIndex(), pos, pos);
        m_matrix.remove_column(pos);
        m_columns.remove(pos);
        endRemoveColumns();
    }

    void setValue(size_t row, size_t column, T value)
    {
        T oldValue = m_matrix[{row, column}];
        if (oldValue == value) {
            return;
        }

        m_matrix[{row, column}] = value;
        emit dataChanged(createIndex(row, column), createIndex(row, column));
    }

    void setMatrix(ptn::matrix::RustMatrix<T>&& matrix)
    {
        beginResetModel();
        m_rows.resize(matrix.nrows());
        for (int i = 0; i < matrix.nrows(); i++) {
            m_rows[i] = QString::number(i);
        }

        m_columns.resize(matrix.ncols());
        for (int i = 0; i < matrix.ncols(); i++) {
            m_columns[i] = QString::number(i);
        }

        m_matrix.operator=(std::move(matrix));
        endResetModel();
    }

    void clear()
    {
        m_rows.clear();
        m_columns.clear();

        m_matrix.clear();
    }

private:
    QList<QString> m_rows;
    QList<QString> m_columns;

    ptn::matrix::RustMatrix<T> m_matrix;
};

#endif // GPNE_NAMED_MATRIX_MODEL_H
