// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Graph.h"
#include <algorithm>

Node* Graph::addNode()
{
    m_nodes.push_back(std::make_unique<Node>());
    return m_nodes.back().get();
}

Node::Node() : m_x(0.), m_y(0.), m_width(1.0), m_height(1.0) {}

const std::vector<Node*>& Node::parents() const noexcept { return m_parents; }

void Node::addParent(Node* parent)
{
    if (parent && std::find(m_parents.begin(), m_parents.end(), parent) == m_parents.end()) {
        m_parents.push_back(parent);
        parent->m_children.push_back(this);
    }
}

const std::vector<Node*>& Node::children() const noexcept { return m_children; }

double Node::x() const noexcept { return m_x; }

void Node::setX(double x) { m_x = x; }

double Node::y() const noexcept { return m_y; }

void Node::setY(double y) { m_y = y; }

double Node::width() const noexcept { return m_width; }

void Node::setWidth(double width) { m_width = width; }

double Node::height() const noexcept { return m_height; }

void Node::setHeight(double height) { m_height = height; }
