#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "Importer.h"
#include "customFormatUtils.h"
#include "FilesystemUtils.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <string>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

using namespace std;
using hrclock = chrono::high_resolution_clock;

static const glm::ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

GLuint checkerTexture;

Importer importer;
MyWindow myWindow("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

// Buffers para los datos de los modelos
vector<float> vertices;
vector<float> uvs;
vector<unsigned int> indices;

void setupProjection(float fov, float aspectRatio, float nearPlane, float farPlane) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspectRatio, nearPlane, farPlane);
}

void setupView() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float eyeX = myWindow.cameraDistance * sin(myWindow.cameraAngleY) * cos(myWindow.cameraAngleX) + myWindow.panX;
    float eyeY = myWindow.cameraDistance * sin(myWindow.cameraAngleX) + myWindow.panY;
    float eyeZ = myWindow.cameraDistance * cos(myWindow.cameraAngleY) * cos(myWindow.cameraAngleX);

    gluLookAt(eyeX, eyeY, eyeZ, myWindow.panX, myWindow.panY, 0.0, 0.0, 1.0, 0.0);
}

static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw std::exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);

    float fov = 45.0f;
    float aspectRatio = static_cast<float>(WINDOW_SIZE.x) / static_cast<float>(WINDOW_SIZE.y);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    setupProjection(fov, aspectRatio, nearPlane, farPlane);
}

// Genera y carga una textura checkerboard por defecto
void generateDefaultCheckerboard() {
    glGenTextures(1, &checkerTexture);
    glBindTexture(GL_TEXTURE_2D, checkerTexture);

    unsigned char checkerData[64 * 64 * 4];
    for (int i = 0; i < 64; ++i) {
        for (int j = 0; j < 64; ++j) {
            int c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
            checkerData[(i * 64 + j) * 4 + 0] = c;
            checkerData[(i * 64 + j) * 4 + 1] = c;
            checkerData[(i * 64 + j) * 4 + 2] = c;
            checkerData[(i * 64 + j) * 4 + 3] = 255;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// Carga modelos y texturas según el archivo dropeado
void handleFileDrop(const char* filePath) {
    std::string path(filePath);
    std::string extension = path.substr(path.find_last_of('.') + 1);

    if (extension == "fbx") {
        if (importer.loadFBX(filePath)) {
            vertices = importer.getVertices();
            uvs = importer.getUVs();
            indices = importer.getIndices();
            saveCustomFormat("Library/Models/model_data.bin", vertices, uvs, indices); // Guardar los datos del modelo
        }
        else {
            fprintf(stderr, "Failed to load FBX model.\n");
        }
    }
    else if (extension == "png" || extension == "jpg" || extension == "jpeg") {
        GLuint texture = importer.loadTexture(filePath);
        if (texture == 0) {
            fprintf(stderr, "Failed to load texture. Using default checkerboard texture.\n");
            generateDefaultCheckerboard();
        }
    }
    else if (extension == "bin") {
        loadCustomFormat(filePath, vertices, uvs, indices);
    }
    else {
        fprintf(stderr, "Unsupported file type: %s\n", filePath);
    }
}

// Dibuja la escena usando los datos de los modelos cargados
void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setupView();

    if (importer.getTextureID() != 0) {  // Bind texture if loaded
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, importer.getTextureID());
    }
    else {
        generateDefaultCheckerboard();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, checkerTexture); // Use checkerboard as default
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices.data());

    if (!uvs.empty()) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, uvs.data());
    }

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D); // Disable texture after drawing
}

// Configura el main loop
int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);
    init_openGL();

    while (myWindow.isOpen()) {
        auto start = hrclock::now();

        if (!myWindow.processEvents()) break;
        myWindow.draw();
        drawScene();
        myWindow.swapBuffers();

        auto elapsed = hrclock::now() - start;
        std::this_thread::sleep_for(FRAME_DT - elapsed);
    }

    SDL_Quit();
    return 0;
}