#include "Renderer.h"
#include <GL/glu.h>
#include <exception>
#include <iostream>

void Renderer::initOpenGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw std::exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);
}

void Renderer::setupProjection(float fov, float aspectRatio, float nearPlane, float farPlane) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspectRatio, nearPlane, farPlane);
}

void Renderer::setupView(float cameraDistance, float cameraAngleX, float cameraAngleY, float panX, float panY) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float eyeX = cameraDistance * sin(cameraAngleY) * cos(cameraAngleX) + panX;
    float eyeY = cameraDistance * sin(cameraAngleX) + panY;
    float eyeZ = cameraDistance * cos(cameraAngleY) * cos(cameraAngleX);
    gluLookAt(eyeX, eyeY, eyeZ, panX, panY, 0.0, 0.0, 1.0, 0.0);
}