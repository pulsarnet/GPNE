// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef REACHABILITYDOCUMENT_H
#define REACHABILITYDOCUMENT_H

#include <corelib/IDocument.h>

class ReachabilityGraphScene;
class PetriNetDocument;
namespace ptn::modules::reachability
{
    struct Reachability;
}

class ReachabilityDocument : public IDocument {
    Q_OBJECT
public:

    using graph_t = ptn::modules::reachability::Reachability;

    ReachabilityDocument();

    QString displayName() const override;

    bool save(const FileRef &targetPath, QString &errorString) override;
    bool reload(const FileRef &sourcePath, QString &errorString) override;

    QString documentType() const override;

    void setGraph(graph_t* graph) noexcept;
    graph_t* graph() const noexcept;

private:

    /** need store one scene for split view **/
    ReachabilityGraphScene* m_scene = nullptr;
    graph_t* m_graph = nullptr;
    bool m_needReload = true;
};

#endif //REACHABILITYDOCUMENT_H
