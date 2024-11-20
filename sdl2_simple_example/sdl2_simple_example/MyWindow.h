#pragma once


#include "Renderer.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>



#include <string>
#include <vector>

class IEventProcessor {
public:
    virtual void processEvent(const SDL_Event& event) = 0;
};

class MyWindow {
    SDL_Window* _window = nullptr;
    void* _ctx = nullptr;

    unsigned short _width = 800;
    unsigned short _height = 600;

public:
    int width() const { return _width; }
    int height() const { return _height; }
    double aspect() const { return static_cast<double>(_width) / _height; }

    float cameraDistance = 5.0f; // Distancia inicial de la cámara (zoom)
    float cameraAngleX = 0.0f; // Ángulo de rotación en el eje X
    float cameraAngleY = 0.0f; // Ángulo de rotación en el eje Y
    float panX = 0.0f, panY = 0.0f; // Desplazamiento de la cámara (pan)
    bool isSceneOn = true;
    bool isConsolaOn = true;
    bool isConfigOn = true;
    bool isJerarquiaOn = true;
    bool isInspectorOn = true;
    float camSpeed = 0.1;
    //FPS
    float fps = 0.0f;
    int frameCount = 0;
    Uint32 lastTime = 0;
    SDL_Window* windowPtr() const { return _window; }
    void* contextPtr() const { return _ctx; }

    MyWindow(const char* title, unsigned short width, unsigned short height);
    MyWindow(MyWindow&&) noexcept = delete;
    MyWindow(const MyWindow&) = delete;
    MyWindow& operator=(const MyWindow&) = delete;
    ~MyWindow();

    void open(const char* title, unsigned short width, unsigned short height);
    void close();
    bool isOpen() const { return _window; }

    bool processEvents(IEventProcessor* event_processor = nullptr);
    void draw();
    void swapBuffers() const;

    void setupFramebuffer();
    void renderToFramebuffer();

    GLuint getFramebufferTexture() const { return textureColorbuffer; }

    void logMessage(const std::string& message);  // Método para agregar mensajes al LOG
    std::vector<std::string> logMessages;         // Vector de mensajes de LOG
    void UpdateDirectoryContents();

    glm::ivec2 selectedResolution;

    bool isFullscreen = false;

    GLuint framebuffer, textureColorbuffer, rbo;

};

extern void handleFileDrop(const char* filePath);