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
using ivec2 = glm::ivec2;

static const ivec2 WINDOW_SIZE(800, 600);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

Importer importer;
Scene scene;
FileSystemUtils fileSystemUtils;

void handleFileDrop(const char* filePath) {
    std::string fileName = FileSystemUtils::getFileName(filePath);

    size_t dotPos = fileName.find_last_of(".");
    if (dotPos != std::string::npos) {
        fileName = fileName.substr(0, dotPos);
    }
    std::string path(filePath);
    std::string extension = path.substr(path.find_last_of('.') + 1);

    if (extension == "fbx") {
        if (importer.loadFBX(filePath)) {
            scene.loadModelData(importer.getVertices(), importer.getUVs(), importer.getIndices(), fileName);

            // Asigna la textura checker independientemente de si el modelo tiene una textura propia
            if (scene.checkerTextureID == 0)
            {
                scene.checkerTextureID = importer.createCheckerTexture();
            }
            scene.setCheckerTexture(scene.checkerTextureID);
        }
    }
    else if (extension == "png" || extension == "jpg") {
        Texture* texture = importer.loadTexture(filePath);
        if (texture != 0) {
            scene.setTexture(texture);
        }
    }
}

int main(int argc, char** argv) {

    fileSystemUtils.createRequiredDirectories();

    MyWindow myWindow("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
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
    Renderer::initOpenGL(WINDOW_SIZE);
    Renderer::setupProjection(45.0f, 1.0f, 0.1f, 1000.0f);
    myWindow.logMessage("OpenGL context initialized.");

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    handleFileDrop("Library/Meshes/BakerHouse.fbx");
    handleFileDrop("Library/Textures/Baker_house.png");

    while (myWindow.processEvents() && myWindow.isOpen()) {
        auto start = hrclock::now();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers

        Renderer::setupView(myWindow.cameraDistance, myWindow.cameraAngleX, myWindow.cameraAngleY, myWindow.panX, myWindow.panY);

        Renderer::drawGrid(10.0f);  // Draw grid first

        scene.drawScene();         // Draw objects in the scene

        myWindow.draw();
        myWindow.swapBuffers();

        auto elapsed = hrclock::now() - start;
        std::this_thread::sleep_for(FRAME_DT - elapsed);
    }

    SDL_Quit();
    myWindow.logMessage("Application terminated.");
    return 0;
}