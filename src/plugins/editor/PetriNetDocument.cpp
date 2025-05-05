// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "PetriNetDocument.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "Constants.h"
#include "model/JsonFormat.h"
#include "model/NetModel.h"

#include <QDir>

PetriNetDocument::PetriNetDocument() {
    m_model = new NetModel;
    initialize();
}

PetriNetDocument::PetriNetDocument(NetModel* model) : IDocument(), m_model(model) {
    initialize();
}

QString PetriNetDocument::documentType() const {
    return Constants::PETRI_NET_DOC_TYPE;
}

QString PetriNetDocument::displayName() const {
    QString name(file().filename());
    if (name.isEmpty()) {
        qDebug() << "PetriNetDocument has no name";
    }
    return name.isEmpty() ? "Untitled" : file().filename();
}

bool PetriNetDocument::save(const FileRef &targetPath, QString &errorString) {
    FileRef path = targetPath.isEmpty() ? file() : targetPath;
    if (path.isEmpty()) {
        errorString = "File path is empty";
        return false;
    }

    QString dirPath = path.dir();
    QDir dir(dirPath);
    if (!dir.exists()) {
        if (!dir.mkpath(dirPath)) {
            errorString = "Directory mkpath failed: " + dirPath;
            return false;
        }
    }

    QFile file(path.absolutePath());
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        errorString = "Cannot open file for writing: " + path.absolutePath();
        return false;
    }

    QJsonObject net;
    JsonFormat::write(m_model, net);
    QJsonDocument doc(net);
    file.write(doc.toJson());

    setFile(path);
    setModified(false);

    return true;
}

bool PetriNetDocument::reload(const FileRef &sourcePath, QString &errorString) {
    FileRef path = sourcePath.isEmpty() ? file() : sourcePath;
    if (path.isEmpty()) {
        errorString = "File path is empty";
        return false;
    }

    QFile file(path.absolutePath());
    if (!file.open(QIODevice::ReadOnly)) {
        errorString = "Cannot open file for reading";
        return false;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Error parsing JSON:" << error.errorString();
        return false;
    }
    if (doc.isNull()) {
        return true;
    }

    m_reloading = true;
    JsonFormat::load(m_model, doc.object());
    m_reloading = false;

    setFile(path);
    setModified(false);
    return true;
}

NetModel* PetriNetDocument::model() const noexcept {
    return m_model;
}

void PetriNetDocument::onNetModelChanged() {
    if (m_reloading)
        return;

    setModified(true);
    emit contentChanged();
}

void PetriNetDocument::initialize() {
    connect(m_model, &NetModel::vertexInserted, this, &PetriNetDocument::onNetModelChanged);
    connect(m_model, &NetModel::vertexChanged, this, &PetriNetDocument::onNetModelChanged);
    connect(m_model, &NetModel::vertexRemoved, this, &PetriNetDocument::onNetModelChanged);
    connect(m_model, &NetModel::edgeInserted, this, &PetriNetDocument::onNetModelChanged);
    connect(m_model, &NetModel::edgeChanged, this, &PetriNetDocument::onNetModelChanged);
    connect(m_model, &NetModel::edgeRemoved, this, &PetriNetDocument::onNetModelChanged);
}

