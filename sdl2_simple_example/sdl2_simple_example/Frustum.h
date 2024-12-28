#pragma once
#include <glm/glm.hpp>
#include "AABB.h"

class Frustum {
public:
    enum Plane {
        Near = 0,
        Far,
        Left,
        Right,
        Top,
        Bottom,
        Count
    };

    struct FrustumPlane {
        glm::vec3 normal;
        float distance;
    };

    FrustumPlane planes[Plane::Count];

    void update(const glm::mat4& viewProjection) {
        // Extract frustum planes from view-projection matrix
        for (int i = 0; i < 4; ++i) {
            planes[Right].normal[i] = viewProjection[i][3] + viewProjection[i][0];
            planes[Left].normal[i] = viewProjection[i][3] - viewProjection[i][0];
            planes[Top].normal[i] = viewProjection[i][3] - viewProjection[i][1];
            planes[Bottom].normal[i] = viewProjection[i][3] + viewProjection[i][1];
            planes[Far].normal[i] = viewProjection[i][3] - viewProjection[i][2];
            planes[Near].normal[i] = viewProjection[i][3] + viewProjection[i][2];
        }

        // Normalize planes
        for (int i = 0; i < Plane::Count; ++i) {
            float length = glm::length(planes[i].normal);
            planes[i].normal /= length;
            planes[i].distance /= length;
        }
    }

    bool isBoxVisible(const AABB& aabb) const {
        for (int i = 0; i < Plane::Count; ++i) {
            const auto& plane = planes[i];

            glm::vec3 positive = aabb.min;
            if (plane.normal.x >= 0) positive.x = aabb.max.x;
            if (plane.normal.y >= 0) positive.y = aabb.max.y;
            if (plane.normal.z >= 0) positive.z = aabb.max.z;

            float d = glm::dot(plane.normal, positive) + plane.distance;
            if (d < 0) return false;
        }
        return true;
    }
};
