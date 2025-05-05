// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef NEWNETDIALOG_H
#define NEWNETDIALOG_H

#include <QDialog>
#include <QPointer>
#include <corelib/Project.h>

class QLabel;
class QLineEdit;

class NewNetDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewNetDialog(QPointer<Project> project, QWidget *parent = nullptr);

    QString name() const noexcept;

private slots:
    void onAccept();

private:
    QLineEdit *m_lineEdit;
    QLabel    *m_errorLabel;
    QPointer<Project> m_project;

    QSet<QString> m_existingFilesSet;
};



#endif //NEWNETDIALOG_H
