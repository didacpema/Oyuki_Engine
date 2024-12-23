#include "QuadtreeNode.h"
#include <GL/glew.h>

QuadtreeNode::QuadtreeNode(const AABB& bounds, int maxDepth)
    : bounds(bounds), maxDepth(maxDepth), currentDepth(0), isLeaf(true) {
    for (int i = 0; i < 4; i++) {
        children[i] = nullptr;
    }
}

QuadtreeNode::~QuadtreeNode() {
    clear();
}

void QuadtreeNode::split() {
    if (!isLeaf) return;

    glm::vec3 center = bounds.getCenter();
    glm::vec3 extents = bounds.getExtents() * 0.5f;

    // Create four children - divide only XZ plane (quad)
    children[0] = new QuadtreeNode(
        AABB(bounds.min, glm::vec3(center.x, bounds.max.y, center.z)),
        maxDepth
    );
    children[1] = new QuadtreeNode(
        AABB(glm::vec3(center.x, bounds.min.y, bounds.min.z),
            glm::vec3(bounds.max.x, bounds.max.y, center.z)),
        maxDepth
    );
    children[2] = new QuadtreeNode(
        AABB(glm::vec3(bounds.min.x, bounds.min.y, center.z),
            glm::vec3(center.x, bounds.max.y, bounds.max.z)),
        maxDepth
    );
    children[3] = new QuadtreeNode(
        AABB(glm::vec3(center.x, bounds.min.y, center.z), bounds.max),
        maxDepth
    );

    for (int i = 0; i < 4; i++) {
        children[i]->currentDepth = currentDepth + 1;
    }

    // Redistribute existing objects to children
    for (auto obj : objects) {
        int quadrant = getQuadrant(obj->getAABB());
        if (quadrant != -1) {
            children[quadrant]->insert(obj);
        }
    }

    isLeaf = false;
    objects.clear();
}

void QuadtreeNode::insert(GameObject* obj) {
    if (!bounds.intersects(obj->getAABB())) return;

    if (isLeaf) {
        objects.push_back(obj);

        // Split if we exceed capacity and haven't reached max depth
        if (objects.size() > MAX_OBJECTS && currentDepth < maxDepth) {
            split();
        }
    }
    else {
        int quadrant = getQuadrant(obj->getAABB());
        if (quadrant != -1) {
            children[quadrant]->insert(obj);
        }
        else {
            objects.push_back(obj);
        }
    }
}

std::vector<GameObject*> QuadtreeNode::queryRange(const AABB& range) {
    std::vector<GameObject*> found;

    if (!bounds.intersects(range)) return found;

    for (auto obj : objects) {
        if (obj->getAABB().intersects(range)) {
            found.push_back(obj);
        }
    }

    if (!isLeaf) {
        for (int i = 0; i < 4; i++) {
            auto childFound = children[i]->queryRange(range);
            found.insert(found.end(), childFound.begin(), childFound.end());
        }
    }

    return found;
}
int QuadtreeNode::getQuadrant(const AABB& objBounds) const {
    glm::vec3 center = bounds.getCenter();
    glm::vec3 objCenter = objBounds.getCenter();

    // If object spans multiple quadrants, return -1
    if (objBounds.min.x < center.x && objBounds.max.x > center.x ||
        objBounds.min.z < center.z && objBounds.max.z > center.z) {
        return -1;
    }

    // Determine quadrant based on XZ position
    if (objCenter.x < center.x) {
        if (objCenter.z < center.z) return 0;  // Bottom-left
        return 2;  // Top-left
    }
    else {
        if (objCenter.z < center.z) return 1;  // Bottom-right
        return 3;  // Top-right
    }
}
void QuadtreeNode::clear() {
    objects.clear();
    for (int i = 0; i < 4; i++) {
        if (children[i]) {
            delete children[i];
            children[i] = nullptr;
        }
    }
    isLeaf = true;

}
void QuadtreeNode::debugDraw() const {
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(bounds.min.x, 0, bounds.min.z);
    glVertex3f(bounds.max.x, 0, bounds.min.z);
    glVertex3f(bounds.max.x, 0, bounds.max.z);
    glVertex3f(bounds.min.x, 0, bounds.max.z);
    glEnd();

    if (!isLeaf) {
        for (int i = 0; i < 4; i++) {
            if (children[i]) children[i]->debugDraw();
        }
    }
}