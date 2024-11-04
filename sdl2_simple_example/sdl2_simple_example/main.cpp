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

Importer importer;
Scene scene;

void handleFileDrop(const char* filePath) {
    std::string fileName = FileSystemUtils::getFileName(filePath);
    std::string path(filePath);
    std::string extension = path.substr(path.find_last_of('.') + 1);
    
    if (extension == "fbx") {
        if (importer.loadFBX(filePath)) {
            scene.loadModelData(importer.getVertices(), importer.getUVs(), importer.getIndices(), fileName);

            // Asigna la textura checker usando el nuevo método
            if (importer.getTextureID() == 0) {
                GLuint checkerTexture = importer.createCheckerTexture();
                scene.setCheckerTexture(checkerTexture);
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
    MyWindow myWindow("SDL2 Simple Example", 1024, 720);
    importer.setWindow(&myWindow);

    myWindow.logMessage("Initializing SDL...");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        myWindow.logMessage("Error initializing SDL: " + std::string(SDL_GetError()));
        return -1;
    }
    myWindow.logMessage("SDL initialized successfully.");

    myWindow.logMessage("Initializing DevIL...");
    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);
    myWindow.logMessage("DevIL initialized successfully.");

    myWindow.logMessage("Initializing OpenGL context...");
    Renderer::initOpenGL();
    Renderer::setupProjection(45.0f, 1.0f, 0.1f, 100.0f);
    myWindow.logMessage("OpenGL context initialized.");

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    while (myWindow.processEvents() && myWindow.isOpen()) {
        auto start = hrclock::now();

        Renderer::setupView(myWindow.cameraDistance, myWindow.cameraAngleX, myWindow.cameraAngleY, myWindow.panX, myWindow.panY);
        scene.drawScene();

        myWindow.draw();
        myWindow.swapBuffers();

        auto elapsed = hrclock::now() - start;
        std::this_thread::sleep_for(FRAME_DT - elapsed);
    }

    SDL_Quit();
    myWindow.logMessage("Application terminated.");
    return 0;
}