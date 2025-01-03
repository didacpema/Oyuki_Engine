#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include <vector>

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
};