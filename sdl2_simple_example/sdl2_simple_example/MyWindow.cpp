
#include "MyWindow.h"
#include <exception>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <algorithm>

#include "Consola.h"
#include "Config.h"
#include "Jerarquia.h"
#include "Escena.h"
#include "Inspector.h"
#include "FileExplorer.h"

using namespace std;
extern Scene scene;
Renderer renderer;
ImGuiIO* g_io = nullptr;
Inspector inspector;
Explorer explorer;

float x = Renderer::eyeX;
float y = Renderer::eyeY;
float z = Renderer::eyeZ;
bool chekerOn = false;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height) : fps(0.0f), frameCount(0), lastTime(SDL_GetTicks()) {
    SDL_Init(SDL_INIT_VIDEO);  // Mueve esta l�nea al principio del constructor
    open(title, width, height);
    
    ImGui::CreateContext();

    g_io = &ImGui::GetIO();
    g_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard 
    g_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
    g_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable multiple viewports

    ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Tema de colores a un estilo lila oscuro cercano al negro
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.02f, 0.1f, 0.9f);          // Fondo de la ventana
    colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.05f, 0.2f, 1.0f);           // Título de la ventana (sin focus)
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.05f, 0.25f, 1.0f);    // Título de la ventana (con focus)
    colors[ImGuiCol_Border] = ImVec4(0.2f, 0.1f, 0.25f, 1.0f);             // Bordes
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.02f, 0.15f, 1.0f);          // Fondo de widgets como InputFloat3
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.1f, 0.25f, 1.0f);     // Fondo de widgets cuando están en hover
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.1f, 0.3f, 1.0f);      // Fondo de widgets cuando están activos
    colors[ImGuiCol_Button] = ImVec4(0.15f, 0.05f, 0.2f, 1.0f);            // Botones
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.1f, 0.25f, 1.0f);      // Botones cuando están en hover
    colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.1f, 0.3f, 1.0f);       // Botones cuando están activos
    colors[ImGuiCol_Header] = ImVec4(0.2f, 0.1f, 0.25f, 1.0f);             // Cabecera de listas desplegables
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.1f, 0.3f, 1.0f);      // Cabecera de listas desplegables en hover
    colors[ImGuiCol_HeaderActive] = ImVec4(0.3f, 0.15f, 0.35f, 1.0f);      // Cabecera de listas desplegables activas

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
}

MyWindow::~MyWindow() {
    // Limpieza de ImGui
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    // Liberar recursos OpenGL
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &rbo);

    // Cerrar ventana y SDL
    close();
    SDL_Quit();
}

void MyWindow::logMessage(const std::string& message) {
    logMessages.push_back(message);
}

void MyWindow::open(const char* title, unsigned short width, unsigned short height) {
    if (isOpen()) return;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if (!_window) {
        logMessage("Error al crear ventana SDL."); // Mensaje en caso de error
        throw exception(SDL_GetError());
    }

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) {
        logMessage("Error al crear contexto OpenGL."); // Mensaje en caso de error
        throw exception(SDL_GetError());
    }
    logMessage("Ventana y contexto creados con exito.");
}

void MyWindow::close() {
    if (!isOpen()) return;

    SDL_GL_DeleteContext(_ctx);
    _ctx = nullptr;

    SDL_DestroyWindow(static_cast<SDL_Window*>(_window));
    _window = nullptr;

    logMessage("Ventana cerrada.");
}

void MyWindow::setupFramebuffer() {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderer._WINDOW_SIZE.x, renderer._WINDOW_SIZE.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, renderer._WINDOW_SIZE.x, renderer._WINDOW_SIZE.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        logMessage("Framebuffer no está completo.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MyWindow::renderToFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        logMessage("Framebuffer no está completo.");
        return;
    }

    renderer.setupView(cameraDistance, cameraAngleX, cameraAngleY, panX, panY);

    renderer.deployGrid(10.0f);  // Draw grid first

    scene.drawScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MyWindow::draw() {
    Config config(this);

    Uint32 currentTime = SDL_GetTicks();
    frameCount++;

    // Calcular FPS cada segundo
    if (currentTime - lastTime >= 1000) {
        fps = frameCount / ((currentTime - lastTime) / 1000.0f);
        frameCount = 0;
        lastTime = currentTime;
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Barra de men� principal
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            if (ImGui::MenuItem("Salir")) {
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            if (ImGui::MenuItem("GitHub")) {
                SDL_OpenURL("https://github.com/didacpema/Oyuki_Engine.git");
                // Aqu� podr�amos abrir el enlace de GitHub
            }
            if (ImGui::MenuItem("Sobre el motor")) {
                ImGui::OpenPopup("AboutPopup");
                ImGui::Text("Este motor grafico es propiedad de los estudiantes de la UPC");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Editor")) {
            if (ImGui::MenuItem("Consola", NULL, isConsolaOn, true))
            {
                isConsolaOn = !isConsolaOn;
            }
            if (ImGui::MenuItem("Config", NULL, isConfigOn, true))
            {
                isConfigOn = !isConfigOn;
            }
            if (ImGui::MenuItem("Jerarquia", NULL, isJerarquiaOn, true))
            {
                isJerarquiaOn = !isJerarquiaOn;
            }
            if (ImGui::MenuItem("Inspector", NULL, isInspectorOn, true))
            {
                isInspectorOn = !isInspectorOn;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject")) {
            if (ImGui::MenuItem("Cube")) {
                scene.createCube("Library/Meshes/cube.fbx");
            }
            if (ImGui::MenuItem("Sphere")) {
                scene.createSphere("Sphere");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    //COMENTA ESTA LINEA PARA SER FELIZ
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

    if (isConsolaOn)
    {
        Consola::draw(logMessages);
    }

    if (isConfigOn)
    {
        config.draw(_window, renderer, fps);
    }

    if (isJerarquiaOn) {
        Jerarquia::draw();
    }

    // Si el usuario pulsa 'F', movemos la cámara al objeto seleccionado
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F)) && scene.selectedGameObjectIndex != -1) {
        GameObject* selectedObject = scene.gameObjects[scene.selectedGameObjectIndex];
        panX = selectedObject->transform.position.x;
        panY = selectedObject->transform.position.y;
        cameraDistance = glm::distance(glm::vec3(x, y, z), selectedObject->transform.position); // Ajuste de distancia
        logMessage("Cámara ajustada al objeto seleccionado.");
    }

    if (isSceneOn) {
        renderToFramebuffer();  // Renderiza la escena al framebuffer
        Escena::draw(textureColorbuffer);
    }

    if (isInspectorOn)
    {
        inspector.draw();
    }


    // Popup "About"
    if (ImGui::BeginPopup("AboutPopup")) {
        ImGui::Text("Informacion sobre el motor:");
        ImGui::Text("Version: 1.0");
        ImGui::Text("Desarrollado con SDL, OpenGL, ImGui, Assimp, DevIL");
        ImGui::EndPopup();
    }

    explorer.draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (g_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(_window, _ctx);
    }

}

void MyWindow::swapBuffers() const {
    SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));
}

bool MyWindow::processEvents(IEventProcessor* event_processor) {
    SDL_Event e;
    static int lastMouseX, lastMouseY;

    while (SDL_PollEvent(&e)) {
        // Enviar el evento a ImGui primero
        ImGui_ImplSDL2_ProcessEvent(&e);

        if (event_processor) {
            event_processor->processEvent(e);  // Procesar eventos personalizados
        }

        switch (e.type) 
        {
        case SDL_DROPFILE: {
            char* droppedFile = e.drop.file;
            logMessage("Archivo arrastrado: " + string(droppedFile));
            handleFileDrop(droppedFile); // Tu función para manejar archivos
            SDL_free(droppedFile);
            break;
        }
        case SDL_QUIT:
            logMessage("Evento de salida recibido.");
            close();
            return false;
        case SDL_MOUSEMOTION:
            if (e.motion.state & SDL_BUTTON_RMASK) {
                if (SDL_GetModState() & KMOD_ALT) {
                    cameraAngleY += e.motion.xrel * 0.005f;
                    cameraAngleX -= e.motion.yrel * 0.005f;
                }
            }
            break;
        case SDL_KEYDOWN:
            // Movimiento de cámara existente
            if (e.key.keysym.sym == SDLK_w) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                    panY += camSpeed * 2;
                else
                    panY += camSpeed;
            }
            if (e.key.keysym.sym == SDLK_s) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                    panY -= camSpeed * 2;
                else
                    panY -= camSpeed;
            }
            if (e.key.keysym.sym == SDLK_a) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                    panX -= camSpeed * 2;
                else
                    panX -= camSpeed;
            }
            if (e.key.keysym.sym == SDLK_d) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                    panX += camSpeed * 2;
                else
                    panX += camSpeed;
            }

            // Nueva funcionalidad para enfocar el objeto seleccionado con tecla 'F'
            if (e.key.keysym.sym == SDLK_f) {
                // Verifica que haya un objeto seleccionado
                if (scene.selectedGameObjectIndex >= 0 && scene.selectedGameObjectIndex < scene.gameObjects.size()) {
                    GameObject* selectedObject = scene.gameObjects[scene.selectedGameObjectIndex];

                    // Cambia el pivote de la cámara a la posición del objeto seleccionado
                    panX = selectedObject->transform.position.x;
                    panY = selectedObject->transform.position.y;
                    cameraDistance = glm::distance(glm::vec3(x, y, z), selectedObject->transform.position); // Ajuste de distancia

                    logMessage("Cámara centrada en el objeto seleccionado.");
                }
                else {
                    logMessage("No hay objeto seleccionado.");
                }
            }
            break;
        case SDL_MOUSEWHEEL:
            cameraDistance += e.wheel.y > 0 ? -0.5f : 0.5f;
            cameraDistance = max(1.0f, cameraDistance);
            break;
        }
    }
    return true;
}

