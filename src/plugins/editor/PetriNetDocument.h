// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PETRINETDOCUMENT_H
#define PETRINETDOCUMENT_H

#include <corelib/IDocument.h>

class NetModel;

class PetriNetDocument final : public IDocument {

    Q_OBJECT

public:

    PetriNetDocument();
    explicit PetriNetDocument(NetModel* model);

    QString documentType() const override;

    QString displayName() const override;

    bool save(const FileRef &targetPath, QString &errorString) override;
    bool reload(const FileRef &sourcePath, QString &errorString) override;

    NetModel* model() const noexcept;

public slots:

    void onNetModelChanged();

protected:

    void initialize();

private:

    NetModel* m_model;
    bool m_reloading = false;
};

#endif //PETRINETDOCUMENT_H
