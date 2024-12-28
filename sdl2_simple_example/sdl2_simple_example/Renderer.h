#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "MyWindow.h"
#include <vector>
#include "Frustum.h"
#include "Scene.h"
using ivec2 = glm::ivec2;

class Renderer {
public:
    static void initOpenGL(ivec2 WINDOW_SIZE);
    static void setupProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
    static void setupView(float cameraDistance, float cameraAngleX, float cameraAngleY, float panX, float panY);
    static void deployGrid(float spacing);
    void applyProjectionAndViewport(ivec2 selectedResolution);

    // Nuevas variables miembro
    static float eyeX, eyeY, eyeZ;

    static ivec2 _WINDOW_SIZE;

    Frustum frustum;
    bool showBoundingBoxes = false;

    void updateFrustum() {
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(_WINDOW_SIZE.x) / _WINDOW_SIZE.y, 0.1f, 1000.0f);

        glm::mat4 view = glm::lookAt(
            glm::vec3(eyeX, eyeY, eyeZ),
            glm::vec3(panX, panY, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)
        );

        frustum.update(projection * view);
    }
};