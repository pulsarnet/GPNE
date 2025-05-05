// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SugiyamaLayout.h"
#include "Graph.h"
#include <algorithm>
#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>
#include <cstddef>

/**
 * @brief Structure representing a node in the Sugiyama layout algorithm.
 */
struct SugiyamaNode final {
    std::vector<SugiyamaNode*> children; ///< List of child nodes
    std::vector<SugiyamaNode*> parents;  ///< List of parent nodes
    int layer = -1;                      ///< Layer of the node in the layout
    int order = -1;                      ///< Order of the node within its layer
    Node* m_node;                        ///< Pointer to the original node

    /**
     * @brief Constructor to initialize a SugiyamaNode with a given node.
     * @param node Pointer to the original node.
     */
    explicit SugiyamaNode(Node* node) : m_node(node) {}

    void setX(double x) { m_node->setX(x); }
    double x() const noexcept { return m_node->x(); }

    void setY(double y) { m_node->setY(y); }
    double y() const noexcept { return m_node->y(); }

    void setWidth(double width) { m_node->setWidth(width); }
    double width() const noexcept { return m_node->width(); }

    void setHeight(double height) { m_node->setHeight(height); }
    double height() const noexcept { return m_node->height(); }
};

/**
 * @brief Assign layers to the nodes in the graph.
 * @param graph Vector of SugiyamaNode pointers representing the graph.
 */
void assignLayers(std::vector<SugiyamaNode*>& graph)
{
    std::vector<std::size_t> inDegree(graph.size(), 0);
    for (auto* node : graph) {
        inDegree[node->layer] = node->parents.size();
    }

    std::queue<SugiyamaNode*> q;
    for (auto* node : graph) {
        if (inDegree[node->layer] == 0) {
            node->layer = 0;
            q.push(node);
        }
    }

    while (!q.empty()) {
        SugiyamaNode* u = q.front();
        q.pop();
        int currentLayer = u->layer;
        for (auto* child : u->children) {
            inDegree[child->layer]--;
            child->layer = std::max(child->layer, currentLayer + 1);
            if (inDegree[child->layer] == 0) {
                q.push(child);
            }
        }
    }
}

/**
 * @brief Build layers from the graph.
 * @param graph Vector of SugiyamaNode pointers representing the graph.
 * @return Vector of vectors of SugiyamaNode pointers, each inner vector representing a layer.
 */
std::vector<std::vector<SugiyamaNode*>> buildLayers(const std::vector<SugiyamaNode*>& graph)
{
    int maxLayer = -1;
    for (auto* node : graph) {
        maxLayer = std::max(maxLayer, node->layer);
    }
    std::vector<std::vector<SugiyamaNode*>> layers(maxLayer + 1);
    for (auto* node : graph) {
        layers[node->layer].push_back(node);
    }
    return layers;
}

/**
 * @brief Reorder nodes within a layer based on a barycenter function.
 * @param layer Vector of SugiyamaNode pointers representing a layer.
 * @param baryFunc Function to calculate the barycenter of a node.
 */
void reorderLayer(std::vector<SugiyamaNode*>& layer, std::function<double(SugiyamaNode*)> baryFunc)
{
    std::sort(layer.begin(), layer.end(), [&](SugiyamaNode* a, SugiyamaNode* b) {
        return baryFunc(a) < baryFunc(b);
    });
    for (int i = 0; i < layer.size(); ++i) {
        layer[i]->order = i;
    }
}

/**
 * @brief Calculate the barycenter of a node's parents.
 * @param v Pointer to a SugiyamaNode.
 * @return Barycenter value.
 */
double barycenterOfParents(SugiyamaNode* v)
{
    if (v->parents.empty()) {
        return v->order;
    }
    double sum = 0.0;
    for (auto* p : v->parents) {
        sum += p->order;
    }
    return sum / v->parents.size();
}

/**
 * @brief Calculate the barycenter of a node's children.
 * @param v Pointer to a SugiyamaNode.
 * @return Barycenter value.
 */
double barycenterOfChildren(SugiyamaNode* v)
{
    if (v->children.empty()) {
        return v->order;
    }
    double sum = 0.0;
    for (auto* c : v->children) {
        sum += c->order;
    }
    return sum / v->children.size();
}

/**
 * @brief Minimize crossings between edges in the graph.
 * @param layers Vector of vectors of SugiyamaNode pointers, each inner vector representing a layer.
 */
void crossingMinimization(std::vector<std::vector<SugiyamaNode*>>& layers) {
    for (auto& layer : layers) {
        for (std::size_t i = 0; i < layer.size(); ++i) {
            layer[i]->order = (int)i;
        }
    }

    const size_t ITERATIONS = 10;
    for (std::size_t iter = 0; iter < ITERATIONS; ++iter) {
        for (std::size_t i = 1; i < layers.size(); ++i) {
            reorderLayer(layers[i], barycenterOfParents);
        }

        if (layers.size() > 1) {
            for (std::size_t i = layers.size() - 2; i > 0; --i) {
                reorderLayer(layers[i], barycenterOfChildren);
            }
        }
    }
}

/**
 * @brief Assign coordinates to the nodes in the graph.
 * @param layers Vector of vectors of SugiyamaNode pointers, each inner vector representing a layer.
 * @param layerHeight Height of each layer.
 * @param nodeSpacing Spacing between nodes.
 */
void assignCoordinates(
    std::vector<std::vector<SugiyamaNode*>>& layers,
    double layerHeight = 1.0,
    double nodeSpacing = 1.0
)
{
    for (size_t L = 0; L < layers.size(); ++L) {
        for (size_t i = 0; i < layers[L].size(); ++i) {
            SugiyamaNode* v = layers[L][i];
            v->setY(L * layerHeight);
            v->setX(v->order * nodeSpacing);
        }
    }
}

/**
 * @brief Apply the Sugiyama layout algorithm to a graph starting from the root node.
 * @param root Pointer to the root node of the graph.
 */
void sugiyamaLayout(Node* root)
{
    std::vector<SugiyamaNode*> graph;
    auto createSugiyamaNode = [&](Node* node) {
        auto* snode = new SugiyamaNode(node);
        graph.push_back(snode);
        return snode;
    };

    std::queue<Node*> q;
    q.push(root);
    std::unordered_map<Node*, SugiyamaNode*> nodeMap;
    nodeMap[root] = createSugiyamaNode(root);

    while (!q.empty()) {
        Node* current = q.front();
        q.pop();
        SugiyamaNode* sCurrent = nodeMap[current];

        for (Node* child : current->children()) {
            if (nodeMap.find(child) == nodeMap.end()) {
                nodeMap[child] = createSugiyamaNode(child);
                q.push(child);
            }
            SugiyamaNode* sChild = nodeMap[child];
            sCurrent->children.push_back(sChild);
            sChild->parents.push_back(sCurrent);
        }
    }

    assignLayers(graph);
    auto layers = buildLayers(graph);
    crossingMinimization(layers);
    assignCoordinates(layers);

    std::ranges::for_each(graph, [](SugiyamaNode* node) { delete node; });
}
