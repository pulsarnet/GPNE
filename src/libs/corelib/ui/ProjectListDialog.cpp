// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ProjectListDialog.h"

#include <QCoreApplication>
#include <QMargins>
#include <QLabel>
#include <QMenu>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>
#include <QPushButton>

#include "ProjectListItem.h"
#include <corelib/MainWindow.h>
#include <corelib/ActionManager.h>
#include <corelib/Constants.h>
#include <corelib/RecentProjectsManager.h>

ProjectListDialog::ProjectListDialog(MainWindow* parent)
    : QWidget(parent)
    , m_mainWindow(parent)
{
    QHBoxLayout* outerLayout = new QHBoxLayout(this);
    QVBoxLayout* innerMainLayout = new QVBoxLayout();
    outerLayout->addStretch();
    outerLayout->addLayout(innerMainLayout);
    outerLayout->addStretch();

    outerLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    innerMainLayout->setContentsMargins(QMargins(0, 0, 0, 0));

    auto outerProjectLayout = new QVBoxLayout();
    m_projectsLayout = new QVBoxLayout();
    outerProjectLayout->addLayout(m_projectsLayout);
    outerProjectLayout->addStretch(0);

    m_projectsLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    outerProjectLayout->setContentsMargins(QMargins(0, 0, 0, 0));

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setLayout(outerProjectLayout);

    QLabel* applicationTitle = new QLabel(QCoreApplication::applicationName(), this);
    QFont font;
    font.setBold(true);
    font.setPointSize(28);
    applicationTitle->setFont(font);

    m_createProjectButton = new QToolButton(this);
    m_createProjectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_createProjectButton->setText(tr("New Project"));
    m_createProjectButton->setIcon(QIcon(":/images/icons/create_project.svg"));
    m_createProjectButton->setAutoRaise(true);
    m_createProjectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_createProjectButton->setCursor(Qt::PointingHandCursor);
    m_createProjectButton->setMinimumSize(QSize(48, 48));
    m_createProjectButton->setDefaultAction(ActionManager::action(Constants::A_NEW_PROJECT));

    m_openProjectButton = new QToolButton(this);
    m_openProjectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_openProjectButton->setText(tr("Open Project"));
    m_openProjectButton->setIcon(QIcon(":/images/icons/file_open.svg"));
    m_openProjectButton->setAutoRaise(true);
    m_openProjectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_openProjectButton->setCursor(Qt::PointingHandCursor);
    m_openProjectButton->setMinimumSize(QSize(48, 48));
    m_openProjectButton->setDefaultAction(ActionManager::action(Constants::A_OPEN_PROJECT));

    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_createProjectButton, Qt::AlignLeft);
    buttonLayout->addWidget(m_openProjectButton, Qt::AlignLeft);

    connect(RecentProjectsManager::instance(), &RecentProjectsManager::projectsChanged, this, &ProjectListDialog::onProjectsChanged);
    connect(m_createProjectButton, &QPushButton::clicked, this, &ProjectListDialog::createProject);
    connect(m_openProjectButton, &QPushButton::clicked, this, &ProjectListDialog::openProject);

    innerMainLayout->addStretch(0);
    innerMainLayout->addWidget(applicationTitle);
    innerMainLayout->addLayout(buttonLayout);
    innerMainLayout->addWidget(m_scrollArea);
    innerMainLayout->addStretch(0);

    onProjectsChanged();
}

void ProjectListDialog::onProjectsChanged() {
    for (auto widget : m_projectListItems) {
        m_projectsLayout->removeWidget(widget);
        widget->deleteLater();
    }
    m_projectListItems.clear();

    for (auto project : RecentProjectsManager::instance()->getProjects()) {
        auto frame = new ProjectListItem(m_mainWindow, project.name, project.path, this);
        m_projectsLayout->addWidget(frame);
        m_projectListItems.push_back(frame);
    }
}

