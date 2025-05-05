// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ReachabilityDocument.h"
#include "Constants.h"
#include <editor/model/NetModel.h>
#include <editor/PetriNetDocument.h>

#include "ReachabilityGraphScene.h"

// ReachabilityDocument is a document type where only state document
// If it deletes/changed in filesystem no reload needed because its a service document
// But it can save state
// Its never modified state change, because only modified documents must ask for save

ReachabilityDocument::ReachabilityDocument() : IDocument() {
    m_scene = new ReachabilityGraphScene(this);
}

QString ReachabilityDocument::displayName() const {
    return tr("Reachability Tree");
}

bool ReachabilityDocument::save(const FileRef &targetPath, QString &errorString) {
    Q_UNUSED(targetPath);
    Q_UNUSED(errorString);
    // save nodes and places of graph
    return true;
}

bool ReachabilityDocument::reload(const FileRef &sourcePath, QString &errorString) {
    Q_UNUSED(sourcePath);
    Q_UNUSED(errorString);
    return true;
}

QString ReachabilityDocument::documentType() const {
    return Constants::REACHABILITY_DOC_TYPE;
}

void ReachabilityDocument::setGraph(graph_t *graph) noexcept {
    if (m_graph != graph) {
        m_graph = graph;
        emit contentChanged();
    }
}

ReachabilityDocument::graph_t * ReachabilityDocument::graph() const noexcept {
    return m_graph;
}
