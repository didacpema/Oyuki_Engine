#include "Escena.h"

extern Renderer renderer;

Escena::Escena(){}
Escena::~Escena(){}

void Escena::draw(GLuint textureColorbuffer) {
    ImGui::Begin("Scene");
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH")) {
            const char* filePath = (const char*)payload->Data;
           // std::cout << "Dropped file: " << filePath << std::endl;
            handleFileDrop(filePath); // Procesar archivo arrastrado
        }
        ImGui::EndDragDropTarget();
    }
    if (textureColorbuffer) {
        ImGui::Image((void*)(intptr_t)textureColorbuffer, ImVec2(renderer._WINDOW_SIZE.x, renderer._WINDOW_SIZE.y));
    }
    ImGui::End();
}
