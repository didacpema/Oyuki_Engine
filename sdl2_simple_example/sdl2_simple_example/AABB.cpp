#include "AABB.h"
#include <algorithm>

bool AABB::contains(const AABB& other) const {
    return min.x <= other.min.x && min.y <= other.min.y && min.z <= other.min.z &&
        max.x >= other.max.x && max.y >= other.max.y && max.z >= other.max.z;
}

bool AABB::intersects(const AABB& other) const {
    return min.x <= other.max.x && max.x >= other.min.x &&
        min.y <= other.max.y && max.y >= other.min.y &&
        min.z <= other.max.z && max.z >= other.min.z;
}

AABB AABB::merge(const AABB& other) const {
    return AABB(
        glm::vec3(
            std::min(min.x, other.min.x),
            std::min(min.y, other.min.y),
            std::min(min.z, other.min.z)
        ),
        glm::vec3(
            std::max(max.x, other.max.x),
            std::max(max.y, other.max.y),
            std::max(max.z, other.max.z)
        )
    );
}