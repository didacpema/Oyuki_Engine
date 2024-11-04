#include "scene.h"
#include <exception>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include "MyWindow.h"
<<<<<<< Updated upstream
=======
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <cmath>
#include <algorithm>


>>>>>>> Stashed changes
using namespace std;
extern Scene scene;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height){
	open(title, width, height);
}

MyWindow::~MyWindow() {
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
<<<<<<< Updated upstream
=======

    logMessage("Ventana cerrada.");
}

void MyWindow::draw() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Barra de menú principal
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            if (ImGui::MenuItem("Salir")) {
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            if (ImGui::MenuItem("GitHub")) {
                // Aquí podríamos abrir el enlace de GitHub
            }
            if (ImGui::MenuItem("Sobre el motor")) {
                ImGui::OpenPopup("AboutPopup");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Editor")) {
            ImGui::MenuItem("Consola", NULL, true);
            ImGui::MenuItem("Config", NULL, true);
            ImGui::MenuItem("Jerarquia", NULL, true);
            ImGui::MenuItem("Inspector", NULL, true);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Formes bàsiques")) {
            if (ImGui::MenuItem("Cargar Forma Bàsica")) {
                // Aquí se manejará la carga de formas
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Consola (vacía, sin contenido de LOG aún)
    ImGui::Begin("Consola");
    for (const auto& msg : logMessages) {
        ImGui::Text("%s", msg.c_str());
    }
    ImGui::End();

    // Configuración (vacía)
    ImGui::Begin("Config");
    ImGui::Text("Configuració general del sistema");

    // Placeholder para gráfico de FPS
    ImGui::Text("Gràfic FPS (Placeholder)");
    static float values[90] = {};
    static int values_offset = 0;
    values[values_offset] = 60.0f; // Ejemplo fijo de FPS
    values_offset = (values_offset + 1) % 90;
    ImGui::PlotLines("FPS", values, IM_ARRAYSIZE(values), values_offset, "FPS", 0.0f, 100.0f, ImVec2(0, 80));

    // Opciones de configuración de módulos (placeholder)
    ImGui::Text("Configuració de cada mòdul:");
    ImGui::Text(" - Renderitzador\n - Finestra\n - Entrada\n - Textures");

    // Placeholder para consumo de memoria y detección de hardware
    ImGui::Text("Consum de Memòria: ");
    ImGui::Text("Detecció de maquinari i versions de programari:");
    ImGui::Text("SDL, OpenGL, DevIL");
    ImGui::End();

    // Jerarquía (vacía)
    ImGui::Begin("Jerarquia");
    ImGui::Text("Llista de GameObjects:");
    for (size_t i = 0; i < scene.gameObjectNames.size(); ++i) {
        ImGui::Text("%s", scene.gameObjectNames[i].c_str()); // Mostrar el nombre del objeto
    }
    ImGui::Text("Aquí se mostrará la jerarquía de GameObjects");
    ImGui::End();

    // Inspector (vacío)
    ImGui::Begin("Inspector");
    ImGui::Text("Informació del GameObject seleccionat");
    ImGui::Text("Component: Transform");
    ImGui::Text("Posició: (0,0,0)");
    ImGui::Text("Rotació: (0,0,0)");
    ImGui::Text("Escala: (1,1,1)");
    ImGui::Text("Component: Mesh");
    ImGui::Text("Informació de la malla carregada");
    ImGui::Text("Component: Texture");
    ImGui::Text("Mida de la textura i camí");
    ImGui::End();

    // Popup "About"
    if (ImGui::BeginPopup("AboutPopup")) {
        ImGui::Text("Informació sobre el motor:");
        ImGui::Text("Versió: 1.0");
        ImGui::Text("Desenvolupat amb SDL, OpenGL, ImGui, Assimp, DevIL");
        ImGui::EndPopup();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
>>>>>>> Stashed changes
}

void MyWindow::swapBuffers() const {
    SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));
}

bool MyWindow::processEvents(IEventProcessor* event_processor) {
<<<<<<< Updated upstream
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (event_processor) event_processor->processEvent(e);
		switch (e.type) {
		case SDL_QUIT: close(); return false;
		}
=======
    SDL_Event e;
    static int lastMouseX, lastMouseY;

    while (SDL_PollEvent(&e)) {
        // Enviar el evento a ImGui primero
        ImGui_ImplSDL2_ProcessEvent(&e);

        // Procesa el evento de archivo arrastrado y soltado
        if (e.type == SDL_DROPFILE) {
            char* droppedFile = e.drop.file;
            logMessage("Archivo arrastrado: " + string(droppedFile));
            handleFileDrop(droppedFile); // Tu función para manejar archivos
            SDL_free(droppedFile);
            continue;
        }

        if (event_processor) {
            event_processor->processEvent(e);  // Procesar eventos personalizados
        }

        switch (e.type) {
        case SDL_QUIT:
            logMessage("Evento de salida recibido.");
            close();
            return false;
        case SDL_MOUSEMOTION:
            if (e.motion.state & SDL_BUTTON_RMASK) {
                if (SDL_GetModState() & KMOD_ALT) {
                    cameraAngleY -= e.motion.xrel * 0.005f;
                    cameraAngleX += e.motion.yrel * 0.005f;
                }
               
            }
            if (e.motion.state & e.key.keysym.sym == SDLK_w) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panY += cameraSpeed * 2;
                }
                else
                {
                    panY += cameraSpeed;
                }


            }
            if (e.motion.state & e.key.keysym.sym == SDLK_s) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panY -= cameraSpeed * 2;
                }
                else
                {
                    panY -= cameraSpeed;
                }


            }
            if (e.motion.state & e.key.keysym.sym == SDLK_a) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panX -= cameraSpeed * 2;
                }
                else
                {
                    panX -= cameraSpeed;
                }
            }
            if (e.motion.state & e.key.keysym.sym == SDLK_d) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panX += cameraSpeed * 2;
                }
                else
                {
                    panX += cameraSpeed;
                }


            }
            break;
		case SDL_KEYDOWN:
			if(e.key.keysym.sym == SDLK_w) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panY += cameraSpeed*2;
				}
				else
				{
					panY += cameraSpeed;
				}
                    
                
			}
			if (e.key.keysym.sym == SDLK_s) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
					panY -= cameraSpeed * 2;
				}
				else
				{
					panY -= cameraSpeed;
				}
					
				
			}
			if (e.key.keysym.sym == SDLK_a) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panX -= cameraSpeed * 2;
                }
                else
				{
					panX -= cameraSpeed;
				}
			}
			if (e.key.keysym.sym == SDLK_d) {
				if (e.key.keysym.mod & KMOD_SHIFT)
				{
					panX += cameraSpeed * 2;
				}
                else
                {
                    panX += cameraSpeed;
                }
					
				
			}
			
			break;
        case SDL_MOUSEWHEEL:
            cameraDistance += e.wheel.y > 0 ? -0.5f : 0.5f;
            cameraDistance = max(1.0f, cameraDistance);
            break;
        }
>>>>>>> Stashed changes
    }
    return true;
}

