// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ProjectExplorerModel.h"

#include <QIcon>
#include "Constants.h"
#include "ProjectFileIndexer.h"
#include <corelib/Project.h>

struct TreeNode {
    explicit TreeNode(const QString& name, const FileRef& file, TreeNode* parent = nullptr);
    ~TreeNode();

    QString m_name;
    FileRef m_file;
    QList<TreeNode*> m_children;
    TreeNode* m_parent = nullptr;
};

TreeNode::TreeNode(const QString& name, const FileRef& file, TreeNode* parent)
{
    m_file = file;
    m_name = name;
    m_parent = parent;

    if (m_parent)
        m_parent->m_children.append(this);
}

TreeNode::~TreeNode()
{
    qDeleteAll(m_children);
}

ProjectExplorerModel::ProjectExplorerModel(Project* project) : QAbstractItemModel(project)
{
    m_project = project;
    m_root = new TreeNode("Project", FileRef());

    connect(ProjectFileIndexer::instance(project), &ProjectFileIndexer::indexChanged, this, &ProjectExplorerModel::build);
    build("nets");
}

QModelIndex ProjectExplorerModel::index(int row, int column, const QModelIndex& parent) const
{
    TreeNode *parentNode = nullptr;
    if (!parent.isValid()) {
        parentNode = m_root;
    } else {
        parentNode = static_cast<TreeNode*>(parent.internalPointer());
    }

    if (!parentNode)
        return {};

    if (row < 0 || row >= parentNode->m_children.size())
        return {};

    TreeNode *childNode = parentNode->m_children.at(row);
    if (!childNode)
        return {};

    return createIndex(row, column, childNode);
}

QModelIndex ProjectExplorerModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    TreeNode *childNode = static_cast<TreeNode*>(child.internalPointer());
    if (!childNode)
        return QModelIndex();

    TreeNode *parentNode = childNode->m_parent;
    if (!parentNode || parentNode == m_root)
        return QModelIndex();

    TreeNode *grandParent = parentNode->m_parent;
    if (!grandParent)
        return QModelIndex();

    int row = grandParent->m_children.indexOf(parentNode);
    if (row < 0)
        return QModelIndex();

    return createIndex(row, 0, parentNode);
}

int ProjectExplorerModel::rowCount(const QModelIndex& parent) const
{
    TreeNode *parentNode = nullptr;
    if (!parent.isValid()) {
        parentNode = m_root;
    } else {
        parentNode = static_cast<TreeNode*>(parent.internalPointer());
    }

    if (!parentNode)
        return 0;

    return parentNode->m_children.size();
}

int ProjectExplorerModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ProjectExplorerModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (index.column() != 0)
        return QVariant();

    TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
    if (!node)
        return QVariant();

    switch (role) {
        case Qt::DisplayRole:
            return node->m_name;
        case Qt::DecorationRole:
            return QIcon(Constants::GRAPH_ICON);
        case Qt::UserRole:
            return QVariant::fromValue(node->m_file);
        default:
            return {};
    }
}


void ProjectExplorerModel::clear()
{
    delete m_root;
}

void ProjectExplorerModel::build(QString index)
{
    if (index != "nets") {
        return;
    }

    beginResetModel();
    qDeleteAll(m_root->m_children);
    m_root->m_children.clear();

    auto files = ProjectFileIndexer::files(m_project, index);
    for (const FileRef &file : files) {
        new TreeNode(file.filename(), file, m_root);
    }
    endResetModel();
}
