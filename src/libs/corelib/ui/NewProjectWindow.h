// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_NEWPROJECTWINDOW_H
#define GPNE_NEWPROJECTWINDOW_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class NewProjectWindow : public QDialog {

    Q_OBJECT

public:
    explicit NewProjectWindow(QWidget* parent = nullptr);

    const QString& projectDirPath() const noexcept;

protected slots:

    void onBrowserButtonClicked();
    void onCreateButtonClicked();
    void updateState();

private:
    QLineEdit* m_dirPath;        // Select file field
    QLineEdit* m_projectName;    // Project name field
    QPushButton* m_createButton; // Create button
    QPushButton* m_cancelButton; // Cancel button
    QString m_projectDirPath;
};

#endif // GPNE_NEWPROJECTWINDOW_H
