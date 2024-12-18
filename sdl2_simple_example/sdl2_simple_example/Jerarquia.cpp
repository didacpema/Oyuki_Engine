#include "Jerarquia.h"


extern Scene scene;

Jerarquia::Jerarquia() {}
Jerarquia::~Jerarquia() {}

void Jerarquia::draw() {
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
            }
        }
    }
    ImGui::End();
}