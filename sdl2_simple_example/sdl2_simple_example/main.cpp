

#define SDL_MAIN_HANDLED

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
#include "FileExplorer.h"

using namespace std;
using hrclock = chrono::high_resolution_clock;
using ivec2 = glm::ivec2;

static const ivec2 WINDOW_SIZE(800, 600);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

Importer importer;
Scene scene;
MyWindow myWindow("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
extern Explorer explorer;

void handleFileDrop(const char* filePath) {
    std::string fileName = FileSystemUtils::getFileName(filePath);

    // Procesar extensiones
    size_t dotPos = fileName.find_last_of(".");
    if (dotPos != std::string::npos) {
        fileName = fileName.substr(0, dotPos);
    }
    std::string path(filePath);
    std::string extension = path.substr(path.find_last_of('.') + 1);

    if (extension == "fbx") {
        if (importer.loadFBX(filePath)) {
            scene.loadModelData(importer.getVertices(), importer.getUVs(), importer.getIndices(), fileName);

            // Configurar textura checker si no est� asignada

            scene.checkerTextureID = importer.GenerateCheckerTexture();
            
            scene.setCheckerTexture(scene.checkerTextureID);
        }
    }
    else if (extension == "png" || extension == "jpg" || extension == "tga") {
        TextureData* texture = importer.loadTexture(filePath);
        if (texture != nullptr) {
            scene.setTexture(texture);
        }
    }

    // Actualizar el contenido del directorio en la interfaz
    explorer.UpdateDirectoryContents();
}

int main(int argc, char** argv) {

    FileSystemUtils::GenerateRequiredDirectories();
    
    explorer.UpdateDirectoryContents(); // Cargar contenido inicial
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
    myWindow.setupFramebuffer();
    myWindow.renderToFramebuffer();

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    handleFileDrop("Library/Meshes/Street.fbx");
    handleFileDrop("Library/Textures/building_06_c.tga");
    handleFileDrop("Library/Textures/building_016_c.tga");
    handleFileDrop("Library/Textures/building_025_c.tga");
    handleFileDrop("Library/Textures/building01_c.tga");
    handleFileDrop("Library/Textures/building03_c.tga");
    handleFileDrop("Library/Textures/building05_c.tga");


    while (myWindow.processEvents() && myWindow.isOpen()) {
        auto start = hrclock::now();
        
        

        myWindow.draw();
        myWindow.swapBuffers();

        auto elapsed = hrclock::now() - start;
        std::this_thread::sleep_for(FRAME_DT - elapsed);
    }

    SDL_Quit();
    myWindow.logMessage("Application terminated.");
    return 0;
}