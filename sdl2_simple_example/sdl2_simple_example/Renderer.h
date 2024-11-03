#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Renderer {
public:
    static void initOpenGL();
    static void setupProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
    static void setupView(float cameraDistance, float cameraAngleX, float cameraAngleY, float panX, float panY);
};