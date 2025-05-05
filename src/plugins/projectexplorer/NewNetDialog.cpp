// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "NewNetDialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <qvalidator.h>
#include <QVBoxLayout>
#include <corelib/EditorManager.h>
#include <corelib/IDocument.h>
#include <corelib/Project.h>
#include <corelib/FileRef.h>

#include "Constants.h"
#include "ProjectFileIndexer.h"
#include "../editor/Constants.h"

static QRegularExpression validNameRegex("^[a-zA-Z0-9_ ]+$");

NewNetDialog::NewNetDialog(QPointer<Project> project, QWidget *parent)
        : QDialog(parent)
        , m_project(project)
{
    setWindowTitle(tr("Enter Petri Net name:"));

    auto layout = new QVBoxLayout(this);

    QLabel *promptLabel = new QLabel("Petri net name:", this);
    layout->addWidget(promptLabel);

    m_lineEdit = new QLineEdit(this);
    layout->addWidget(m_lineEdit);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: red;");
    layout->addWidget(m_errorLabel);

    auto buttonBox = new QDialogButtonBox(
                QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                Qt::Horizontal,
                this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &NewNetDialog::onAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NewNetDialog::reject);
    layout->addWidget(buttonBox);


    QList<IDocument*> currentDocuments = EditorManager::documents(project);
    for (const auto& document : currentDocuments) {
        if (document->documentType() == Constants::PETRI_NET_DOC_TYPE) {
            m_existingFilesSet.insert(document->file().filename());
        }
    }

    setLayout(layout);
}

QString NewNetDialog::name() const noexcept
{
    return m_lineEdit->text();
}

void NewNetDialog::onAccept()  {
    if (m_lineEdit->text().isEmpty()) {
        m_errorLabel->setText(tr("Name cannot be empty"));
        return;
    }

    if (!validNameRegex.match(m_lineEdit->text()).hasMatch()) {
        m_errorLabel->setText(tr("Invalid name format"));
        return;
    }

    QSet<QString> existingFilesSet = m_existingFilesSet;
    if (existingFilesSet.contains(m_lineEdit->text())) {
        m_errorLabel->setText(tr("Petri net already exists"));
        return;
    }

    const auto existingFiles = ProjectFileIndexer::files(m_project, Constants::NET_DIR);
    for (const auto& file : existingFiles) {
        if (file.filename() == m_lineEdit->text()) {
            m_errorLabel->setText(tr("Petri net already exists"));
            return;
        }
    }

    accept();
}
