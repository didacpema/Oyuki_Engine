#include "Jerarquia.h"


extern Scene scene;

Jerarquia::Jerarquia() {}
Jerarquia::~Jerarquia() {}

void Jerarquia::draw() {
    // Jerarqu�a (vac�a)
    ImGui::Begin("Jerarqu�a");
    ImGui::Text("Lista de GameObjects:");

    // Recorre todos los objetos en la jerarqu�a
    for (size_t i = 0; i < scene.gameObjectNames.size(); ++i) {
        // Verifica si el objeto est� seleccionado (si el �ndice coincide con el seleccionado)
        bool isSelected = (scene.selectedGameObjectIndex == i);

        // Dibuja el objeto y permite seleccionarlo
        if (ImGui::Selectable((scene.gameObjectNames[i] + "###" + std::to_string(i)).c_str(), isSelected)) {
            // Si el objeto seleccionado es diferente al actual, actualiza la selecci�n
            if (scene.selectedGameObjectIndex != i) {
                // Cambia el �ndice del objeto seleccionado
                scene.selectedGameObjectIndex = i;
            }
        }
    }
    ImGui::End();
}