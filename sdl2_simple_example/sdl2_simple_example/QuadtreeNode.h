#pragma once
#include "GameObject.h"
#include "AABB.h"
#include <vector>

class QuadtreeNode {
public:
    QuadtreeNode(const AABB& bounds, int maxDepth = 5);
    ~QuadtreeNode();

    void insert(GameObject* obj);
    void remove(GameObject* obj);
    std::vector<GameObject*> queryRange(const AABB& range);
    void clear();
    void debugDraw() const;

private:
    static const int MAX_OBJECTS = 8;

    AABB bounds;
    int maxDepth;
    int currentDepth;
    bool isLeaf;
    std::vector<GameObject*> objects;
    QuadtreeNode* children[8];

    void split();
    int getQuadrant(const AABB& objBounds) const;
};