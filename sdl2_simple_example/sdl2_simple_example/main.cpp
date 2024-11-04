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

#include "Scene.h"
#include "Renderer.h"

using namespace std;
using hrclock = chrono::high_resolution_clock;

static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

MyWindow myWindow("SDL2 Simple Example", 512, 512);
Importer importer;
Scene scene;

void handleFileDrop(const char* filePath) {
    std::string path(filePath);
    std::string extension = path.substr(path.find_last_of('.') + 1);

    if (extension == "fbx") {
        if (importer.loadFBX(filePath)) {
            scene.loadModelData(importer.getVertices(), importer.getUVs(), importer.getIndices());

            // Asigna la textura checker si no hay ninguna
            if (importer.getTextureID() == 0) {
                GLuint checkerTexture = importer.createCheckerTexture();
                scene.setTexture(checkerTexture);
            }
            else {
                scene.setTexture(importer.getTextureID());
            }
        }
    }
    else if (extension == "png" || extension == "jpg") {
        GLuint texture = importer.loadTexture(filePath);
        if (texture != 0) {
            scene.setTexture(texture);
        }
    }
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);

    Renderer::initOpenGL();
    Renderer::setupProjection(45.0f, 1.0f, 0.1f, 100.0f);

    while (myWindow.processEvents() && myWindow.isOpen()) {
        auto start = hrclock::now();
        if (!myWindow.processEvents()) break;

        Renderer::setupView(myWindow.cameraDistance, myWindow.cameraAngleX, myWindow.cameraAngleY, myWindow.panX, myWindow.panY);
        scene.drawScene();
        myWindow.draw();
        myWindow.swapBuffers();

        auto elapsed = hrclock::now() - start;
        std::this_thread::sleep_for(FRAME_DT - elapsed);
    }

    SDL_Quit();
    return 0;
}