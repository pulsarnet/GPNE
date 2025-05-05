// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Buhheim.h"
#include "Graph.h"
#include <algorithm>
#include <format>
#include <functional>
#include <optional>
#include <queue>
#include <ranges>

struct BuhheimNode {

    explicit BuhheimNode(Node* node);

    BuhheimNode* left() const;
    BuhheimNode* right() const;

    BuhheimNode* leftBrother() const;
    BuhheimNode* leftMostSibling();

    double x() const { return m_node->x(); }

    void setX(double x) { m_node->setX(x); }

    double y() const { return m_node->y(); }

    void setY(double y) { m_node->setY(y); }

    double width() const { return m_node->width(); }

    void setWidth(double width) { m_node->setWidth(width); }

    void reset(int depth = 0, int index = 1);

    std::size_t subtreeSize() const;
    void reorderSubtree();

    std::vector<BuhheimNode*> children;
    BuhheimNode* parent;
    BuhheimNode* thread;
    double mod;
    double offset;
    BuhheimNode* ancestor;
    double change;
    double shift;
    int position_in_sibling_group;

private:
    BuhheimNode* m_lmost_sibling;
    Node* m_node;
};

BuhheimNode::BuhheimNode(Node* node)
    : parent(nullptr)
    , thread(nullptr)
    , mod(0)
    , ancestor(this)
    , change(0)
    , shift(0)
    , offset(0.)
    , m_lmost_sibling(nullptr)
    , position_in_sibling_group(1)
    , m_node(node)
{
    m_node->setX(-1.);
    m_node->setY(0.);
}

BuhheimNode* BuhheimNode::left() const
{
    if (thread) {
        return thread;
    }

    if (!children.empty()) {
        return children.front();
    }

    return nullptr;
}

BuhheimNode* BuhheimNode::right() const
{
    if (thread) {
        return thread;
    }

    if (!children.empty()) {
        return children.back();
    }

    return nullptr;
}

BuhheimNode* BuhheimNode::leftBrother() const
{
    BuhheimNode* n = nullptr;
    if (parent) {
        for (auto node : parent->children) {
            if (node == this) {
                return n;
            }
            n = node;
        }
    }
    return n;
}

BuhheimNode* BuhheimNode::leftMostSibling()
{
    if (!m_lmost_sibling && parent && this != parent->children[0]) {
        m_lmost_sibling = parent->children[0];
    }
    return m_lmost_sibling;
}

void BuhheimNode::reset(int depth, int index)
{
    m_node->setX(-1.);
    m_node->setY(depth);
    thread = nullptr;
    mod = 0;
    ancestor = this;
    change = 0;
    shift = 0;
    m_lmost_sibling = nullptr;
    position_in_sibling_group = index;

    int idx = 0;
    for (auto& child : children) {
        child->reset(depth + 1, ++idx);
    }
}

std::size_t BuhheimNode::subtreeSize() const
{
    std::size_t total = 1; // includes current node
    for (auto* child : children) {
        total += child->subtreeSize();
    }
    return total;
}

void BuhheimNode::reorderSubtree()
{
    // First, we recursively "arrange" the subtrees of all children
    for (auto* c : children) {
        c->reorderSubtree();
    }

    // Sorting the children in descending order of the size of the subtrees
    std::sort(children.begin(), children.end(), [](BuhheimNode* a, BuhheimNode* b) {
        return a->subtreeSize() > b->subtreeSize();
    });

    // If m_y is odd, expand the order (so that the big ones are "on the right")
    if ((int)m_node->y() % 2 != 0) {
        std::reverse(children.begin(), children.end());
    }
}

BuhheimNode* apportion(BuhheimNode* node, BuhheimNode* default_ancestor, double distance = 5.);
BuhheimNode* firstWalk(BuhheimNode* node, double distance = 5.);
std::optional<double>
secondWalk(BuhheimNode* node, double m = 0, double depth = 0, std::optional<double> min = {});
void moveSubtree(BuhheimNode* wl, BuhheimNode* wr, double shift);
BuhheimNode* ancestor(BuhheimNode* vil, BuhheimNode* node, BuhheimNode* default_ancestor);
void thirdWalk(BuhheimNode* node, double n);
void executeShifts(BuhheimNode* node);

double minDist(BuhheimNode* left, BuhheimNode* right, double margin = 1.0)
{
    return left->width() / 2.0 + right->width() / 2.0 + margin;
}

void centerLeavesAmongSiblings(BuhheimNode* node, double distance = 5.)
{
    // Also correct recursively for each child.
    for (BuhheimNode* c : node->children) {
        centerLeavesAmongSiblings(c, distance);
    }

    auto children = node->children;
    if (children.size() < 2) {
        return;
    }

    // Let's find the nearest non-leaf sibling on the left and right
    std::vector<BuhheimNode*> leafs;
    BuhheimNode* left = nullptr;
    BuhheimNode* right = nullptr;

    for (int i = 0; i < (int)children.size(); i++) {
        if (!children[i]->children.empty()) {
            if (children[i]->x() > node->x()) {
                if (right && children[i]->x() < right->x()) {
                    right = children[i];
                } else if (!right) {
                    right = children[i];
                }
            } else {
                if (left && children[i]->x() > left->x()) {
                    left = children[i];
                } else if (!left) {
                    left = children[i];
                }
            }
        } else {
            leafs.push_back(children[i]);
        }
    }

    // If there are no suitable non-leaf brothers, then do nothing.
    if (!left || !right || leafs.empty()) {
        return;
    }

    // Distributing the leaf vertices in equal increments between left->x() and right->x() closer to the parent
    double leafs_width = leafs.back()->x() - leafs.front()->x();

    double left_min = left->x() + distance + leafs.front()->width();
    double right_max = right->x() - distance - leafs.back()->width();

    double left_x = node->x() - leafs_width / 2.;
    double right_x = node->x() + leafs_width / 2.;

    if (left_x < left_min) {
        left_x = left_min;
        right_x = left_min + leafs_width;
    }

    if (right_x > right_max) {
        right_x = right_max;
        left_x = right_max - leafs_width;
    }

    for (int i = 0; i < (int)leafs.size(); i++) {
        leafs[i]->setX(left_x);

        if (i != (int)leafs.size() - 1) {
            left_x += minDist(leafs[i], leafs[i + 1], distance);
        } else {
            left_x += distance;
        }
    }
}

void buchheim(Node* root)
{
    std::vector<BuhheimNode*> nodes;
    auto createBuhheimNode = [&](Node* node) {
        auto snode = new BuhheimNode(node);
        nodes.push_back(snode);
        return snode;
    };

    BuhheimNode* graph = createBuhheimNode(root);

    std::queue<Node*> q;
    q.push(root);
    std::unordered_map<Node*, BuhheimNode*> nodeMap;
    nodeMap[root] = graph;

    while (!q.empty()) {
        Node* current = q.front();
        q.pop();
        BuhheimNode* sCurrent = nodeMap[current];

        for (Node* child : current->children()) {
            if (nodeMap.find(child) == nodeMap.end()) {
                nodeMap[child] = createBuhheimNode(child);
                q.push(child);
            }
            BuhheimNode* sChild = nodeMap[child];
            sCurrent->children.push_back(sChild);
            sChild->parent = sCurrent; // todo if many parents
        }
    }

    graph->reorderSubtree();
    graph->reset();

    BuhheimNode* dt = firstWalk(graph);
    std::optional min = secondWalk(dt);
    if (min.has_value() && min.value() < 0.) {
        thirdWalk(dt, -min.value());
    }
    centerLeavesAmongSiblings(graph);

    std::ranges::for_each(nodes, [](BuhheimNode* node) { delete node; });
}

void thirdWalk(BuhheimNode* node, double n)
{
    node->setX(node->x() + n);
    for (auto c : node->children) {
        thirdWalk(c, n);
    }
}

BuhheimNode* firstWalk(BuhheimNode* node, double distance)
{
    if (node->children.empty()) {
        if (node->leftMostSibling()) {
            BuhheimNode* w = node->leftBrother();
            double dist = minDist(w, node, distance); // distance between the centers
            node->setX(w->x() + dist);
        } else {
            node->setX(0);
        }
    } else {
        BuhheimNode* default_ancestor = node->children.front();
        for (auto w : node->children) {
            firstWalk(w);
            default_ancestor = apportion(w, default_ancestor, distance);
        }
        executeShifts(node);

        double midpoint = (node->children.front()->x() + node->children.back()->x()) / 2;
        BuhheimNode* w = node->leftBrother(); // distance between the centers
        if (w) {
            double dist = minDist(w, node, distance);
            node->setX(w->x() + dist);
            node->mod = node->x() - midpoint;
        } else {
            node->setX(midpoint);
        }
    }
    return node;
}

BuhheimNode* apportion(BuhheimNode* node, BuhheimNode* default_ancestor, double distance)
{
    BuhheimNode* w = node->leftBrother();
    if (w) {
        BuhheimNode *vir = node, *vor = node;
        BuhheimNode* vil = w;
        BuhheimNode* vol = node->leftMostSibling();
        double sir = node->mod, sor = node->mod;
        double sil = vil->mod;
        double sol = vol->mod;
        while (vil->right() && vir->left()) {
            vil = vil->right();
            vir = vir->left();
            vol = vol->left();
            vor = vor->right();
            vor->ancestor = node;
            double needed = minDist(vil, vir, distance);
            double overlap = (vil->x() + sil) - (vir->x() + sir);
            double shift = overlap + needed;
            if (shift > 0.) {
                moveSubtree(ancestor(vil, node, default_ancestor), node, shift);
                sir += shift;
                sor += shift;
            }
            sil += vil->mod;
            sir += vir->mod;
            sol += vol->mod;
            sor += vor->mod;
        }
        if (vil->right() && !vor->right()) {
            vor->thread = vil->right();
            vor->mod = vor->mod + sil - sor;
        } else {
            if (vir->left() && !vol->left()) {
                vol->thread = vir->left();
                vol->mod = vol->mod + sir - sol;
            }
            default_ancestor = node;
        }
    }
    return default_ancestor;
}

void moveSubtree(BuhheimNode* wl, BuhheimNode* wr, double shift)
{
    int subtrees = wr->position_in_sibling_group - wl->position_in_sibling_group;
    wr->change = wr->change - shift / subtrees;
    wr->shift = wr->shift + shift;
    wl->change = wl->change + shift / subtrees;
    wr->setX(wr->x() + shift);
    wr->mod = wr->mod + shift;
}

void executeShifts(BuhheimNode* node)
{
    double shift = 0, change = 0;
    for (auto w : std::views::reverse(node->children)) {
        w->setX(w->x() + shift);
        w->mod = w->mod + shift;
        change += w->change;
        shift += w->shift + change;
    }
}

BuhheimNode* ancestor(BuhheimNode* vil, BuhheimNode* node, BuhheimNode* default_ancestor)
{
    const auto& children = node->parent->children;
    auto it = std::find(children.begin(), children.end(), vil->ancestor);
    if (it != children.end()) {
        return vil->ancestor;
    }
    return default_ancestor;
}

std::optional<double>
secondWalk(BuhheimNode* node, double m, double depth, std::optional<double> min)
{
    node->setX(node->x() + m);
    node->setY(depth);

    if (!min.has_value() || node->x() < min.value()) {
        min = node->x();
    }

    for (auto w : node->children) {
        min = secondWalk(w, m + node->mod, depth + 1, min);
    }

    return min;
}