#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);
}

<<<<<<< Updated upstream
static void draw_triangle(const u8vec4& color, const vec3& center, double size) {
	glColor4ub(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLES);
	glVertex3d(center.x, center.y + size, center.z);
	glVertex3d(center.x - size, center.y - size, center.z);
	glVertex3d(center.x + size, center.y - size, center.z);
	glEnd();
}

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw_triangle(u8vec4(255, 0, 0, 255), vec3(0.0, 0.0, 0.0), 0.5);
=======
void handleFileDrop(const char* filePath) {
    string fileName = FileSystemUtils::getFileName(filePath);
    string path(filePath);
    string extension = path.substr(path.find_last_of('.') + 1);
    string name = path.substr(path.find_last_of('/') + 1);
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
>>>>>>> Stashed changes
}

int main(int argc, char** argv) {
	MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

<<<<<<< Updated upstream
	init_openGL();
=======
    myWindow.logMessage("Initializing SDL...");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        myWindow.logMessage("Error initializing SDL: " + string(SDL_GetError()));
        return -1;
    }
    myWindow.logMessage("SDL initialized successfully.");
>>>>>>> Stashed changes

	while(window.processEvents() && window.isOpen()) {
		const auto t0 = hrclock::now();
		display_func();
		window.swapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if(dt<FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}

<<<<<<< Updated upstream
	return 0;
=======
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
        this_thread::sleep_for(FRAME_DT - elapsed);
    }

    SDL_Quit();
    myWindow.logMessage("Application terminated.");
    return 0;
>>>>>>> Stashed changes
}