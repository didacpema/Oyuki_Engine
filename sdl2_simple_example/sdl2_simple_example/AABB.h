#pragma once
#include <glm/glm.hpp>

class AABB {
public:
    glm::vec3 min;
    glm::vec3 max;

    AABB() : min(0), max(0) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    bool contains(const AABB& other) const;
    bool intersects(const AABB& other) const;
    AABB merge(const AABB& other) const;
    glm::vec3 getCenter() const { return (min + max) * 0.5f; }
    glm::vec3 getExtents() const { return max - min; }
};