#include "Escena.h"

extern Renderer renderer;

Escena::Escena(){}
Escena::~Escena(){}

void Escena::draw(GLuint textureColorbuffer) {
    ImGui::Begin("Scene");
    if (textureColorbuffer) {
        ImGui::Image((void*)(intptr_t)textureColorbuffer, ImVec2(renderer._WINDOW_SIZE.x, renderer._WINDOW_SIZE.y));
    }
    ImGui::End();
}
