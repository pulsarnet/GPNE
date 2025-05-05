// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ProjectExplorerView.h"
#include "ProjectExplorerModel.h"
#include "corelib/DocumentManager.h"

#include <editor/PetriNetDocument.h>

#include <QHeaderView>
#include <QLabel>
#include <QTreeView>
#include <corelib/EditorManager.h>

ProjectExplorerView::ProjectExplorerView(Project* project)
{
    m_project = project;
}

QString ProjectExplorerView::title()
{
    return tr("Project Explorer");
}

QIcon ProjectExplorerView::icon()
{
    return QIcon(":/images/icons/folder.svg");
}

QToolBar* ProjectExplorerView::toolbar()
{
    return nullptr;
}

QWidget* ProjectExplorerView::widget() {
    auto treeView = new QTreeView;
    treeView->header()->hide();
    treeView->setIndentation(40);
    treeView->setIconSize(QSize(16, 16));
    treeView->setRootIsDecorated(false);
    treeView->setStyleSheet("QTreeView::item { padding: 10px; }");

    treeView->setModel(new ProjectExplorerModel(m_project));

    connect(treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex& index) {
        QVariant data = index.data(Qt::UserRole);
        if (data.canConvert<FileRef>()) {
            FileRef file = data.value<FileRef>();
            IDocument* document = DocumentManager::getDocument(file);
            if (!document) {
                document = new PetriNetDocument();
                QString errorString;
                if (!document->reload(file, errorString)) {
                    qDebug() << "Error: " << errorString;
                    document->deleteLater();
                    return;
                }
                DocumentManager::addDocument(document);
            }
            EditorManager::openEditor(m_project, document);
        } else {
            qDebug() << "Invalid data";
        }
    });
    return treeView;
}

IDockWindow * ProjectExplorerViewFactory::createDockWindow(Project *project) {
    return new ProjectExplorerView(project);
}
