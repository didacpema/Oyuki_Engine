#include "Jerarquia.h"


extern Scene scene;

Jerarquia::Jerarquia() {}
Jerarquia::~Jerarquia() {}

void Jerarquia::drawGameObjectNode(GameObject* obj, const std::string& name, int index) {
    if (!obj) return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // Añadir la bandera Selected si este objeto está seleccionado
    if (scene.selectedGameObjectIndex == index) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Si no tiene hijos, usamos una hoja (node) en lugar de un árbol
    if (obj->getChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool nodeOpen = ImGui::TreeNodeEx((name + "###" + std::to_string(index)).c_str(), flags);

    // Manejo de la selección al hacer clic
    if (ImGui::IsItemClicked()) {
        scene.selectedGameObjectIndex = index;
    }

    // Manejo del Drag and Drop
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        // Guardamos el índice del objeto que estamos arrastrando
        int draggedIndex = index;
        ImGui::SetDragDropPayload("GAMEOBJECT_DND", &draggedIndex, sizeof(int));
        ImGui::Text("Arrastrando: %s", name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DND")) {
            int droppedIndex = *(const int*)payload->Data;

            // Intentar establecer la relación padre-hijo usando Scene
            if (droppedIndex != index) {
                if (scene.setParentChild(droppedIndex, index)) {
                    // La relación se estableció con éxito
                    printf("Objeto %s ahora es hijo de %s\n",
                        scene.gameObjectNames[droppedIndex].c_str(),
                        scene.gameObjectNames[index].c_str());
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (nodeOpen) {
        auto& children = obj->getChildren();
        for (GameObject* child : children) {
            auto it = std::find(scene.gameObjects.begin(), scene.gameObjects.end(), child);
            if (it != scene.gameObjects.end()) {
                int childIndex = std::distance(scene.gameObjects.begin(), it);
                drawGameObjectNode(child, scene.gameObjectNames[childIndex], childIndex);
            }
        }
        ImGui::TreePop();
    }
}


void Jerarquia::draw() {
    ImGui::Begin("Jerarquía");
    ImGui::Text("Lista de GameObjects:");

    // Área para detectar drops en el espacio vacío
    ImGui::BeginChild("GameObjectList", ImVec2(0, 0), true);
    // Permitir que se suelte un objeto en el área vacía de la ventana (para remover padre)
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DND")) {
            int droppedIndex = *(const int*)payload->Data;
            scene.removeFromParent(droppedIndex);
        }
        ImGui::EndDragDropTarget();
    }
    // Dibujar solo los objetos que no tienen padre (objetos raíz)
    for (size_t i = 0; i < scene.gameObjects.size(); ++i) {
        GameObject* obj = scene.gameObjects[i];
        if (!obj->getParent()) {
            drawGameObjectNode(obj, scene.gameObjectNames[i], i);
        }
    }
    ImGui::EndChild();
    ImGui::End();
}