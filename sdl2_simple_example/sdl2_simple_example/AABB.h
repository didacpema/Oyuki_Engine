#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>  // Añade esta línea
#include <vector>
#include "Transform.h"
class AABB {
public:
    glm::vec3 min;
    glm::vec3 max;

    AABB() : min(0.0f), max(0.0f) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    void updateFromVertices(const std::vector<float>& vertices) {
        if (vertices.empty()) return;

        min = glm::vec3(FLT_MAX);
        max = glm::vec3(-FLT_MAX);

        for (size_t i = 0; i < vertices.size(); i += 3) {
            glm::vec3 vertex(vertices[i], vertices[i + 1], vertices[i + 2]);
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }
    }

    void transform(const Transform& transform) {
        // Create transformation matrix
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, transform.position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, transform.scale);

        // Transform corners
        std::vector<glm::vec3> corners = getCorners();
        min = glm::vec3(FLT_MAX);
        max = glm::vec3(-FLT_MAX);

        for (const auto& corner : corners) {
            glm::vec4 transformedCorner = modelMatrix * glm::vec4(corner, 1.0f);
            min = glm::min(min, glm::vec3(transformedCorner));
            max = glm::max(max, glm::vec3(transformedCorner));
        }
    }

    std::vector<glm::vec3> getCorners() const {
        std::vector<glm::vec3> corners;
        corners.reserve(8);
        corners.push_back(glm::vec3(min.x, min.y, min.z));
        corners.push_back(glm::vec3(max.x, min.y, min.z));
        corners.push_back(glm::vec3(min.x, max.y, min.z));
        corners.push_back(glm::vec3(max.x, max.y, min.z));
        corners.push_back(glm::vec3(min.x, min.y, max.z));
        corners.push_back(glm::vec3(max.x, min.y, max.z));
        corners.push_back(glm::vec3(min.x, max.y, max.z));
        corners.push_back(glm::vec3(max.x, max.y, max.z));
        return corners;
    }
};
