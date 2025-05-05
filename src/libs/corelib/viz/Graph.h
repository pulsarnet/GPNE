// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GRAPH_H
#define GRAPH_H
#include <memory>
#include <vector>

class Node;

class Graph {

public:
    Graph() = default;

    Node* addNode();

private:
    std::vector<std::unique_ptr<Node>> m_nodes;
};

class Node final {

public:
    Node();
    ~Node() = default;

    const std::vector<Node*>& parents() const noexcept;
    void addParent(Node* parent);

    const std::vector<Node*>& children() const noexcept;

    double x() const noexcept;
    void setX(double x);

    double y() const noexcept;
    void setY(double y);

    double width() const noexcept;
    void setWidth(double width);

    double height() const noexcept;
    void setHeight(double height);

private:
    std::vector<Node*> m_parents;
    std::vector<Node*> m_children;

    double m_x;
    double m_y;

    double m_width;
    double m_height;
};

#endif // GRAPH_H
