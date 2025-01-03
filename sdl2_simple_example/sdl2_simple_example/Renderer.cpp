#include "Renderer.h"
#include <GL/glu.h>
#include <exception>
#include <iostream>

// Define the static member variable
ivec2 Renderer::_WINDOW_SIZE;

float Renderer::eyeX = 0.0f;  // Asignar un valor inicial
float Renderer::eyeY = 0.0f;  // Asignar un valor inicial
float Renderer::eyeZ = 0.0f;  // Asignar un valor inicial

void Renderer::initOpenGL(ivec2 WINDOW_SIZE) {
    glewInit();
    if (!GLEW_VERSION_3_0) throw std::exception("OpenGL 3.0 API is not available.");

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);          // Set depth function to less or equal
    glClearDepth(1.0f);               // Clear depth buffer with maximum depth value

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Store the window size
    Renderer::_WINDOW_SIZE = WINDOW_SIZE;
}

void Renderer::deployGrid(float spacing) {
    glDisable(GL_TEXTURE_2D);  // Disable texture for the grid lines
    float gridRange = 1000.0f;

    glColor3f(1.0f, 1.0f, 1.0f);  // Set grid color
    glBegin(GL_LINES);

    for (float i = -gridRange; i <= gridRange; i += spacing) {
        glVertex3f(i, 0, -gridRange);
        glVertex3f(i, 0, gridRange);
    }

    for (float i = -gridRange; i <= gridRange; i += spacing) {
        glVertex3f(-gridRange, 0, i);
        glVertex3f(gridRange, 0, i);
    }

    glEnd();
}

void Renderer::setupProjection(float fov, float aspectRatio, float nearPlane, float farPlane) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float windowAspectRatio = static_cast<float>(Renderer::_WINDOW_SIZE.x) / Renderer::_WINDOW_SIZE.y;
    // Use the stored window size for the aspect ratio
    gluPerspective(fov, static_cast<float>(Renderer::_WINDOW_SIZE.x) / Renderer::_WINDOW_SIZE.y, nearPlane, farPlane);
}

void Renderer::setupView(float cameraDistance, float cameraAngleX, float cameraAngleY, float panX, float panY) {
    glClearDepth(1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Actualiza las variables miembro con los valores calculados
    eyeX = cameraDistance * sin(cameraAngleY) * cos(cameraAngleX) + panX;
    eyeY = cameraDistance * sin(cameraAngleX) + panY;
    eyeZ = cameraDistance * cos(cameraAngleY) * cos(cameraAngleX);

    gluLookAt(eyeX, eyeY, eyeZ, panX, panY, 0.0, 0.0, -1.0, 0.0);
}

void Renderer::applyProjectionAndViewport(ivec2 selectedResolution) {
    glViewport(0, 0, selectedResolution.x, selectedResolution.y);  // Actualiza el viewport
    // Aseg�rate de usar selectedResolution para calcular el aspecto
    setupProjection(45.0f, static_cast<float>(selectedResolution.x) / selectedResolution.y, 0.1f, 1000.0f);
}