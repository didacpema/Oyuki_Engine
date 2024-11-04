#include <exception>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <cmath>
#include <algorithm>

using namespace std;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height) {
    SDL_Init(SDL_INIT_VIDEO);  // Mueve esta l�nea al principio del constructor
    open(title, width, height);

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
    ImGui_ImplOpenGL3_Init("#version 130");

    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
}

MyWindow::~MyWindow() {
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    close();
}

void MyWindow::open(const char* title, unsigned short width, unsigned short height) {
    if (isOpen()) return;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    _window = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if (!_window) throw exception(SDL_GetError());

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) throw exception(SDL_GetError());
    if (SDL_GL_MakeCurrent(_window, _ctx) != 0) throw exception(SDL_GetError());
    if (SDL_GL_SetSwapInterval(1) != 0) throw exception(SDL_GetError());
}

void MyWindow::close() {
    if (!isOpen()) return;

    SDL_GL_DeleteContext(_ctx);
    _ctx = nullptr;

    SDL_DestroyWindow(static_cast<SDL_Window*>(_window));
	_window = nullptr;
}

void MyWindow::draw() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Barra de men� principal
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Men�")) {
            if (ImGui::MenuItem("Sortir")) {
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            if (ImGui::MenuItem("GitHub")) {
                // Aqu� podr�amos abrir el enlace de GitHub
            }
            if (ImGui::MenuItem("Sobre el motor")) {
                ImGui::OpenPopup("AboutPopup");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Editor")) {
            ImGui::MenuItem("Mostrar Consola", NULL, true);
            ImGui::MenuItem("Mostrar Configuraci�", NULL, true);
            ImGui::MenuItem("Mostrar Jerarquia", NULL, true);
            ImGui::MenuItem("Mostrar Inspector", NULL, true);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Formes b�siques")) {
            if (ImGui::MenuItem("Cargar Forma B�sica")) {
                // Aqu� se manejar� la carga de formas
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Consola (vac�a, sin contenido de LOG a�n)
    ImGui::Begin("Consola");
    ImGui::Text("Consola: Aqu� se mostrar� el LOG de carga de geometr�as y bibliotecas");
    ImGui::End();

    // Configuraci�n (vac�a)
    ImGui::Begin("Configuraci�");
    ImGui::Text("Configuraci� general del sistema");

    // Placeholder para gr�fico de FPS
    ImGui::Text("Gr�fic FPS (Placeholder)");
    static float values[90] = {};
    static int values_offset = 0;
    values[values_offset] = 60.0f; // Ejemplo fijo de FPS
    values_offset = (values_offset + 1) % 90;
    ImGui::PlotLines("FPS", values, IM_ARRAYSIZE(values), values_offset, "FPS", 0.0f, 100.0f, ImVec2(0, 80));

    // Opciones de configuraci�n de m�dulos (placeholder)
    ImGui::Text("Configuraci� de cada m�dul:");
    ImGui::Text(" - Renderitzador\n - Finestra\n - Entrada\n - Textures");

    // Placeholder para consumo de memoria y detecci�n de hardware
    ImGui::Text("Consum de Mem�ria: ");
    ImGui::Text("Detecci� de maquinari i versions de programari:");
    ImGui::Text("SDL, OpenGL, DevIL");
    ImGui::End();

    // Jerarqu�a (vac�a)
    ImGui::Begin("Jerarquia");
    ImGui::Text("Llista de GameObjects:");
    ImGui::Text("Aqu� se mostrar� la jerarqu�a de GameObjects");
    ImGui::End();

    // Inspector (vac�o)
    ImGui::Begin("Inspector");
    ImGui::Text("Informaci� del GameObject seleccionat");
    ImGui::Text("Component: Transform");
    ImGui::Text("Posici�: (0,0,0)");
    ImGui::Text("Rotaci�: (0,0,0)");
    ImGui::Text("Escala: (1,1,1)");
    ImGui::Text("Component: Mesh");
    ImGui::Text("Informaci� de la malla carregada");
    ImGui::Text("Component: Texture");
    ImGui::Text("Mida de la textura i cam�");
    ImGui::End();

    // Popup "About"
    if (ImGui::BeginPopup("AboutPopup")) {
        ImGui::Text("Informaci� sobre el motor:");
        ImGui::Text("Versi�: 1.0");
        ImGui::Text("Desenvolupat amb SDL, OpenGL, ImGui, Assimp, DevIL");
        ImGui::EndPopup();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

        // Procesa el evento de archivo arrastrado y soltado
        if (e.type == SDL_DROPFILE) {
            char* droppedFile = e.drop.file;
            handleFileDrop(droppedFile); // Tu funci�n para manejar archivos
            SDL_free(droppedFile);
            continue;
        }

        if (event_processor) {
            event_processor->processEvent(e);  // Procesar eventos personalizados
        }

        switch (e.type) {
        case SDL_QUIT:
            close();
            return false;
        case SDL_MOUSEMOTION:
            if (e.motion.state & SDL_BUTTON_RMASK) {
                if (SDL_GetModState() & KMOD_ALT) {
                    panX -= e.motion.xrel * 0.01f;
                    panY += e.motion.yrel * 0.01f;
                }
                else {
                    cameraAngleY += e.motion.xrel * 0.005f;
                    cameraAngleX += e.motion.yrel * 0.005f;
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

