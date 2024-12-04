#include "Consola.h"

Consola::Consola(){}
Consola::~Consola(){}

void Consola::draw(std::vector<std::string> logMessage) {
    ImGui::Begin("Consola");
    for (const auto& msg : logMessage) {
        ImGui::Text("%s", msg.c_str());
    }
    ImGui::End();
}