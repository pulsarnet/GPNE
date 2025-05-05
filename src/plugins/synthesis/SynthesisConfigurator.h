// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SYNTHESISCONFIGURATOR_H
#define SYNTHESISCONFIGURATOR_H

#include <QDialog>

class QScrollArea;
class QVBoxLayout;

class SynthesisConfigurator : public QDialog {

    Q_OBJECT

public:

    explicit SynthesisConfigurator(QWidget* parent = nullptr);

    void addEditor(QWidget*, const QString&, bool canDisable) noexcept;

private:

    void setupButtons();

    QScrollArea* m_scrollArea;
    QVBoxLayout* m_settingsContainerLayout;
};

#endif //SYNTHESISCONFIGURATOR_H
