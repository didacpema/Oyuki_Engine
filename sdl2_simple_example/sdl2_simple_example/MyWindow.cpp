
#include "MyWindow.h"
#include <exception>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "MemoryUsage.h"
#include <cmath>
#include <algorithm>


using namespace std;
extern Scene scene;
Renderer renderer;
ImGuiIO* g_io = nullptr;

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
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
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

    // Crear la textura a la que renderizar
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderer._WINDOW_SIZE.x, renderer._WINDOW_SIZE.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Crear el renderbuffer para almacenar el buffer de profundidad
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, renderer._WINDOW_SIZE.x, renderer._WINDOW_SIZE.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Desenlazar el framebuffer
}

void MyWindow::renderToFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpiar los buffers

    // Configurar y dibujar la escena
    renderer.setupView(5.0f, 0.5f, 0.5f, 0.0f, 0.0f); // Parámetros de la cámara
    renderer.deployGrid(1.0f); // Renderizar una cuadrícula en la escena
    scene.drawScene(); // Aquí debería dibujar toda la escena

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Desenlazar el framebuffer
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
        ImGui::Begin("Consola");
        for (const auto& msg : logMessages) {
            ImGui::Text("%s", msg.c_str());
        }
        ImGui::End();
    }

    if (isConfigOn)
    {
        ImGui::Begin("Config");
        ImGui::Text("Configuracion general del sistema");
		
        // Sección de resolución de pantalla
        ImGui::Text("Resolución de pantalla:");

        struct ResolutionItem {
            ivec2 resolution;
            std::string name;
        };

        std::vector<ResolutionItem> resolutions = {
            { {800, 600}, "800 x 600" },
            { {1024, 768}, "1024 x 768" },
            { {1280, 720}, "1280 x 720" },
            { {1360, 768}, "1360 x 768" },
            { {1366, 768}, "1366 x 768" },
            { {1440, 900}, "1440 x 900" },
            { {1600, 900}, "1600 x 900" },
            { {1920, 1080}, "1920 x 1080" }
        };

        static int selectedResolutionIndex = 0;
        if (ImGui::Combo("Resolucion", &selectedResolutionIndex, [](void* data, int idx, const char** out_text) {
            const std::vector<ResolutionItem>* resolutions = static_cast<const std::vector<ResolutionItem>*>(data);
            *out_text = resolutions->at(idx).name.c_str(); // Return resolution name
            return true;
            }, (void*)&resolutions, resolutions.size())) {
            selectedResolution = resolutions[selectedResolutionIndex].resolution;

            // Update window size
            SDL_SetWindowSize(_window, selectedResolution.x, selectedResolution.y);

            // Ensure we apply the new projection and viewport
            if (!isFullscreen) {
                renderer.applyProjectionAndViewport(selectedResolution); // Aplica la resolución seleccionada
            }
        }

        // Sección de pantalla completa
        if (ImGui::Checkbox("Pantalla completa", &isFullscreen)) {
            if (isFullscreen) {
                SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                logMessage("Pantalla completa activada.");
            }
            else {
                SDL_SetWindowFullscreen(_window, 0);  // 0 means windowed mode
                logMessage("Pantalla completa desactivada.");
            }

            // Get the current window size dynamically after changing fullscreen mode
            int windowWidth, windowHeight;
            SDL_GetWindowSize(_window, &windowWidth, &windowHeight);

            // Apply the new window size for projection and viewport
            renderer.applyProjectionAndViewport(ivec2(windowWidth, windowHeight));
        }

        ImGui::Text("Grafico FPS");
        static float values[90] = {};
        static int values_offset = 0;
        values[values_offset] = fps; // Ejemplo fijo de FPS
        values_offset = (values_offset + 1) % 90;
        char fpsText[16];
        sprintf_s(fpsText, "%d fps", static_cast<int>(fps));
        ImGui::PlotLines("FPS", values, IM_ARRAYSIZE(values), values_offset, fpsText, 0.0f, 100.0f, ImVec2(0, 80));

        ImGui::Text("Consumo de Memoria: ");
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
        ImGui::Text("Deteccion de maquinaria i versions de programario:");
        ImGui::Text("SDL, OpenGL, DevIL");
        ImGui::End();
    }

    if (isJerarquiaOn) {
        // Jerarquía (vacía)
        ImGui::Begin("Jerarquía");
        ImGui::Text("Lista de GameObjects:");

        // Recorre todos los objetos en la jerarquía
        for (size_t i = 0; i < scene.gameObjectNames.size(); ++i) {
            // Verifica si el objeto está seleccionado (si el índice coincide con el seleccionado)
            bool isSelected = (scene.selectedGameObjectIndex == i);

            // Dibuja el objeto y permite seleccionarlo
            if (ImGui::Selectable((scene.gameObjectNames[i] + "###" + std::to_string(i)).c_str(), isSelected)) {
                // Si el objeto seleccionado es diferente al actual, actualiza la selección
                if (scene.selectedGameObjectIndex != i) {
                    // Cambia el índice del objeto seleccionado
                    scene.selectedGameObjectIndex = i;

                    // No cambiamos la cámara aún, solo actualizamos el índice seleccionado
                    logMessage("Objeto seleccionado.");
                }
            }
        }
        ImGui::End();
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
        renderToFramebuffer();  // Renderizar la escena en el framebuffer

        ImGui::Begin("Scene");

        GLuint textureID = textureColorbuffer; // Usamos la textura del framebuffer
        ImGui::Image((void*)(intptr_t)textureID, ImVec2(Renderer::_WINDOW_SIZE.x, Renderer::_WINDOW_SIZE.y));

        ImGui::End();
    }

    if (isInspectorOn)
    {

        // Inspector (vacio)
        ImGui::Begin("Inspector");

        if (scene.selectedGameObjectIndex >= 0 && scene.selectedGameObjectIndex < scene.gameObjects.size()) {
            GameObject* selectedObject = scene.gameObjects[scene.selectedGameObjectIndex];

            ImGui::Text("Texture");
            // Guardar la textura original solo si aún no lo hemos hecho
            static TextureData* originalTexture = selectedObject->texture;

            if (ImGui::Button("Off")) {
                if (!chekerOn)
                {
                    selectedObject->setTexture(new TextureData(scene.checkerTextureID));   // Asignar textura checker
                }
                chekerOn = !chekerOn;  // Cambiar estado de checker
            }
			ImGui::SameLine();
			if (ImGui::Button("On")) {
				if (chekerOn)
				{
					selectedObject->setTexture(originalTexture);  // Restaurar la textura original
				}
                chekerOn = !chekerOn;  // Cambiar estado de checker
			}

            // Mostrar y modificar la posicion
            float position[3] = { selectedObject->transform.position.x, selectedObject->transform.position.y, selectedObject->transform.position.z };
            if (ImGui::InputFloat3("Posicion", position)) {
                selectedObject->transform.position = { position[0], position[1], position[2] };
            }

            // Mostrar y modificar la rotacion
            float rotation[3] = { selectedObject->transform.rotation.x, selectedObject->transform.rotation.y, selectedObject->transform.rotation.z };
            if (ImGui::InputFloat3("Rotacion", rotation)) {
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

            if (selectedObject->texture->getHeight() && selectedObject->texture->getWidth())
            {
				GLuint GetHeight = selectedObject->texture->getHeight();
				GLuint GetWidth = selectedObject->texture->getWidth();
                ImGui::Text("Texture Shape: %u x %u", GetHeight, GetWidth);
            }
            else
            {
				ImGui::Text("Texture Shape: None");
            }
            ImGui::Text("Texture path: %s", selectedObject->texture->getPath().c_str());
            // Mostrar la informacion de la malla
            if (selectedObject->getMesh()) {
                const std::vector<float>& vertices = selectedObject->getMesh()->getVertices();
                const std::vector<float>& uvs = selectedObject->getMesh()->getUVs();
                const std::vector<unsigned int>& indices = selectedObject->getMesh()->getIndices();

                ImGui::Text("Informacion de la Malla:");
                ImGui::Text("Vertices: %zu", vertices.size());
                ImGui::Text("UVs: %zu", uvs.size());
                ImGui::Text("Indices: %zu", indices.size());

                // Mostrar una vista previa opcional
                ImGui::Separator();
                ImGui::Text("Vista previa de vertices:");
                for (size_t i = 0; i < std::min<size_t>(vertices.size(), 9); i += 3) {
                    ImGui::Text("(%f, %f, %f)", vertices[i], vertices[i + 1], vertices[i + 2]);
                }

                ImGui::Text("Vista previa de UVs:");
                for (size_t i = 0; i < std::min<size_t>(uvs.size(), 6); i += 2) {
                    ImGui::Text("(%f, %f)", uvs[i], uvs[i + 1]);
                }

                ImGui::Text("Vista previa de indices:");
                for (size_t i = 0; i < std::min<size_t>(indices.size(), 9); i += 3) {
                    ImGui::Text("(%u, %u, %u)", indices[i], indices[i + 1], indices[i + 2]);
                }
            }
            else {
                ImGui::Text("Malla: Ninguna");
            }
        }
        else {
            ImGui::Text("Seleccione un GameObject de la jerarquia para ver sus propiedades.");
        }

        ImGui::End();
    }


    // Popup "About"
    if (ImGui::BeginPopup("AboutPopup")) {
        ImGui::Text("Informacion sobre el motor:");
        ImGui::Text("Version: 1.0");
        ImGui::Text("Desarrollado con SDL, OpenGL, ImGui, Assimp, DevIL");
        ImGui::EndPopup();
    }


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

