#include "Renderer.h"
#include <GL/glu.h>
#include <exception>
#include <iostream>

// Define the static member variable
ivec2 Renderer::_WINDOW_SIZE;

void Renderer::initOpenGL(ivec2 WINDOW_SIZE) {
    glewInit();
    if (!GLEW_VERSION_3_0) throw std::exception("OpenGL 3.0 API is not available.");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1.0);

    // Store the window size
    Renderer::_WINDOW_SIZE = WINDOW_SIZE;
}

void Renderer::setupProjection(float fov, float aspectRatio, float nearPlane, float farPlane) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Use the stored window size for the aspect ratio
    gluPerspective(fov, static_cast<float>(Renderer::_WINDOW_SIZE.x) / Renderer::_WINDOW_SIZE.y, nearPlane, farPlane);
}

void Renderer::setupView(float cameraDistance, float cameraAngleX, float cameraAngleY, float panX, float panY) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float eyeX = cameraDistance * sin(cameraAngleY) * cos(cameraAngleX) + panX;
    float eyeY = cameraDistance * sin(cameraAngleX) + panY;
    float eyeZ = cameraDistance * cos(cameraAngleY) * cos(cameraAngleX);

    gluLookAt(eyeX, eyeY, eyeZ, panX, panY, 0.0, 0.0, 1.0, 0.0);
}