// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROJECTEXPLORERMODEL_H
#define PROJECTEXPLORERMODEL_H

#include <QAbstractItemModel>

class Project;
struct TreeNode;

class ProjectExplorerModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit ProjectExplorerModel(Project* project);

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;

private:

    TreeNode* m_root = nullptr;
    Project* m_project;

    void clear();
    void build(QString index);
};

#endif //PROJECTEXPLORERMODEL_H
