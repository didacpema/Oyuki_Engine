#include "Jerarquia.h"
#include "Scene.h" // Para acceder a la escena y los objetos

extern Scene scene;

Jerarquia::Jerarquia() {}
Jerarquia::~Jerarquia() {}

void Jerarquia::draw() {
    ImGui::Begin("Jerarquía");
    ImGui::Text("Lista de GameObjects:");

    for (size_t i = 0; i < scene.gameObjectNames.size(); ++i) {
        bool isSelected = (scene.selectedGameObjectIndex == i);

        // Dibuja el objeto y permite seleccionarlo
        if (ImGui::Selectable((scene.gameObjectNames[i] + "###" + std::to_string(i)).c_str(), isSelected)) {
            scene.selectedGameObjectIndex = i;
        }

        // Configura arrastrar y soltar
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("GAMEOBJECT", &i, sizeof(size_t));
            ImGui::Text("%s", scene.gameObjectNames[i].c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
                size_t draggedIndex = *(const size_t*)payload->Data;
                if (draggedIndex != i) {
                    // Convierte el objeto arrastrado en hijo del actual
                    GameObject* draggedObject = scene.gameObjects[draggedIndex];
                    GameObject* targetObject = scene.gameObjects[i];
                    targetObject->addChild(draggedObject);

                    // No eliminamos el objeto arrastrado de la lista raíz, solo lo marcamos
                    // Si es necesario, podrías moverlo a otro contenedor de objetos hijos
                    // O marcarlo como 'no visible' si es una jerarquía de ocultación

                    // Ajusta los índices si es necesario
                    if (scene.selectedGameObjectIndex == draggedIndex) {
                        scene.selectedGameObjectIndex = -1;
                    }
                    else if (scene.selectedGameObjectIndex > draggedIndex) {
                        scene.selectedGameObjectIndex--;
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    }

    ImGui::End();
}
