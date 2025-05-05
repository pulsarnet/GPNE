// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SYNTHESISDOCUMENT_H
#define SYNTHESISDOCUMENT_H

#include <corelib/IDocument.h>

namespace ptn::modules::decompose {
    struct DecomposeContext;
}

class SynthesisDocument : public IDocument {

    Q_OBJECT

public:
    using decomposition_t = ptn::modules::decompose::DecomposeContext;
    using shared_decomposition_t = std::shared_ptr<decomposition_t>;

    explicit SynthesisDocument(shared_decomposition_t);

    QString displayName() const override;

    bool save(const FileRef &targetPath, QString &errorString) override;
    bool reload(const FileRef &sourcePath, QString &errorString) override;

    QString documentType() const override;

    shared_decomposition_t decomposition();

private:

    shared_decomposition_t m_decomposition;
};



#endif //SYNTHESISDOCUMENT_H
