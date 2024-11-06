#include "Scene.h"
#include <exception>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "MemoryUsage.h"
#include <cmath>
#include <algorithm>

using namespace std;
extern Scene scene;
MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height) : fps(0.0f), frameCount(0), lastTime(SDL_GetTicks()) {
    SDL_Init(SDL_INIT_VIDEO);  // Mueve esta línea al principio del constructor
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

void MyWindow::draw() {

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
                isInspectorOn =!isInspectorOn;
            }
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

    if (isConsolaOn)
    {
        ImGui::Begin("Consola");
        for (const auto& msg : logMessages) {
            ImGui::Text("%s", msg.c_str());
        }
        ImGui::End();
    }
    if (isConfigOn)
    {
        ImGui::Begin("Config");
        ImGui::Text("Configuració general del sistema");
        //gráfico de FPS
        ImGui::Text("Gràfic FPS (Placeholder)");
        static float values[90] = {};
        static int values_offset = 0;
        values[values_offset] = fps; // Ejemplo fijo de FPS
        values_offset = (values_offset + 1) % 90;
        char fpsText[16];
        sprintf_s(fpsText, "%d fps", static_cast<int>(fps));
        ImGui::PlotLines("FPS", values, IM_ARRAYSIZE(values), values_offset, fpsText, 0.0f, 100.0f, ImVec2(0, 80));

        // Opciones de configuración de módulos (placeholder)
        ImGui::Text("Configuració de cada mòdul:");
        ImGui::Text(" - Renderitzador\n - Finestra\n - Entrada\n - Textures");

        //consumo de memoria y detección de hardware
        ImGui::Text("Consum de Memòria: ");
        try {
            MemoryInfo memInfo = MemoryUsage::getMemoryInfo();
            ImGui::Separator();
            ImGui::Text("Consumo de Memoria:");
            ImGui::Text("Memoria Total: %llu MB", memInfo.totalMemory);
            ImGui::Text("Memoria Libre: %llu MB", memInfo.freeMemory);
            ImGui::Text("Memoria Usada: %llu MB", memInfo.usedMemory);
            static float totalMemoryValues[90];
            static float freeMemoryValues[90];
            static float usedMemoryValues[90];
            static int memValuesOffset = 0;

            totalMemoryValues[memValuesOffset] = memInfo.totalMemory;
            freeMemoryValues[memValuesOffset] = memInfo.freeMemory;
            usedMemoryValues[memValuesOffset] = memInfo.usedMemory;
            memValuesOffset = (values_offset + 1) % 90;

            ImGui::PlotLines("TotalMem", totalMemoryValues, IM_ARRAYSIZE(totalMemoryValues), memValuesOffset, "TotalMem", 0.0f, 100.0f, ImVec2(0, 80));
            ImGui::PlotLines("freeMem", freeMemoryValues, IM_ARRAYSIZE(freeMemoryValues), memValuesOffset, "FreeMem", 0.0f, 100.0f, ImVec2(0, 80));
            ImGui::PlotLines("UsedMem", usedMemoryValues, IM_ARRAYSIZE(usedMemoryValues), memValuesOffset, "UsedMem", 0.0f, 100.0f, ImVec2(0, 80));
        }
        catch (const std::exception& e) {
            ImGui::Text("Error obteniendo memoria: %s", e.what());
        }
        ImGui::Text("Detecció de maquinari i versions de programari:");
        ImGui::Text("SDL, OpenGL, DevIL");
        ImGui::End();
    }

    if (isJerarquiaOn)
    {
        // Jerarquía (vacía)
        ImGui::Begin("Jerarquia");
        ImGui::Text("Llista de GameObjects:");
        for (size_t i = 0; i < scene.gameObjectNames.size(); ++i) {
            bool isSelected = (scene.selectedGameObjectIndex == i);
            if (ImGui::Selectable(scene.gameObjectNames[i].c_str(), isSelected)) {
                scene.selectedGameObjectIndex = i; // Actualiza el índice del objeto seleccionado
            }
        }
        ImGui::End();
    }
    if (isInspectorOn)
    {

        // Inspector (vacío)
        ImGui::Begin("Inspector");

        if (scene.selectedGameObjectIndex >= 0 && scene.selectedGameObjectIndex < scene.gameObjects.size()) {
            GameObject* selectedObject = scene.gameObjects[scene.selectedGameObjectIndex];

            ImGui::Text("Información del GameObject seleccionado");

            // Mostrar y modificar la posición
            float position[3] = { selectedObject->transform.position.x, selectedObject->transform.position.y, selectedObject->transform.position.z };
            if (ImGui::InputFloat3("Posición", position)) {
                selectedObject->transform.position = { position[0], position[1], position[2] };
            }

            // Mostrar y modificar la rotación
            float rotation[3] = { selectedObject->transform.rotation.x, selectedObject->transform.rotation.y, selectedObject->transform.rotation.z };
            if (ImGui::InputFloat3("Rotación", rotation)) {
                selectedObject->transform.rotation = { rotation[0], rotation[1], rotation[2] };
            }

            // Mostrar y modificar la escala
            float scale[3] = { selectedObject->transform.scale.x, selectedObject->transform.scale.y, selectedObject->transform.scale.z };
            if (ImGui::InputFloat3("Escala", scale)) {
                selectedObject->transform.scale = { scale[0], scale[1], scale[2] };
            }

            // Mostrar el Texture ID (si existe)
            if (selectedObject->getTexture()) {
                GLuint textureID = selectedObject->getTexture()->getTextureID();
                ImGui::Text("Texture ID: %u", textureID);
            }
            else {
                ImGui::Text("Texture ID: None");
            }

            // Mostrar la información de la malla
            if (selectedObject->getMesh()) {
                const std::vector<float>& vertices = selectedObject->getMesh()->getVertices();
                const std::vector<float>& uvs = selectedObject->getMesh()->getUVs();
                const std::vector<unsigned int>& indices = selectedObject->getMesh()->getIndices();

                ImGui::Text("Información de la Malla:");
                ImGui::Text("Vertices: %zu", vertices.size());
                ImGui::Text("UVs: %zu", uvs.size());
                ImGui::Text("Indices: %zu", indices.size());

                // Mostrar una vista previa opcional
                ImGui::Separator();
                ImGui::Text("Vista previa de vértices:");
                for (size_t i = 0; i < std::min<size_t>(vertices.size(), 9); i += 3) {
                    ImGui::Text("(%f, %f, %f)", vertices[i], vertices[i + 1], vertices[i + 2]);
                }

                ImGui::Text("Vista previa de UVs:");
                for (size_t i = 0; i < std::min<size_t>(uvs.size(), 6); i += 2) {
                    ImGui::Text("(%f, %f)", uvs[i], uvs[i + 1]);
                }

                ImGui::Text("Vista previa de índices:");
                for (size_t i = 0; i < std::min<size_t>(indices.size(), 9); i += 3) {
                    ImGui::Text("(%u, %u, %u)", indices[i], indices[i + 1], indices[i + 2]);
                }
            }
            else {
                ImGui::Text("Malla: Ninguna");
            }
        }
        else {
            ImGui::Text("Seleccione un GameObject de la jerarquía para ver sus propiedades.");
        }

        ImGui::End();
    }
    

    // Popup "About"
    if (ImGui::BeginPopup("AboutPopup")) {
        ImGui::Text("Informació sobre el motor:");
        ImGui::Text("Versió: 1.0");
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
                    cameraAngleY += e.motion.xrel * 0.005f;
                    cameraAngleX -= e.motion.yrel * 0.005f;
                }
               
            }
            break;
		case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_w) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panY += camSpeed * 2;
                }
                else
                {
                    panY += camSpeed;
                }


            }
            if (e.key.keysym.sym == SDLK_s) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panY -= camSpeed * 2;
                }
                else
                {
                    panY -= camSpeed;
                }


            }
            if (e.key.keysym.sym == SDLK_a) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panX -= camSpeed * 2;
                }
                else
                {
                    panX -= camSpeed;
                }
            }
            if (e.key.keysym.sym == SDLK_d) {
                if (e.key.keysym.mod & KMOD_SHIFT)
                {
                    panX += camSpeed * 2;
                }
                else
                {
                    panX += camSpeed;
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

