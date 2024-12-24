#include "Jerarquia.h"

extern Scene scene;

Jerarquia::Jerarquia() {}
Jerarquia::~Jerarquia() {}

void Jerarquia::drawGameObjectNode(GameObject* obj, const std::string& name, int index) {
    if (!obj) return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (scene.selectedGameObjectIndex == index) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (obj->getChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool nodeOpen = ImGui::TreeNodeEx((name + "###" + std::to_string(index)).c_str(), flags);

    if (ImGui::IsItemClicked()) {
        scene.selectedGameObjectIndex = index;
    }

    // Handle delete with Delete key
    if (scene.selectedGameObjectIndex == index && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        scene.deleteGameObjectHierarchy(obj);
        if (nodeOpen) ImGui::TreePop();
        return;
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        int draggedIndex = index;
        ImGui::SetDragDropPayload("GAMEOBJECT_DND", &draggedIndex, sizeof(int));
        ImGui::Text("Arrastrando: %s", name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DND")) {
            int droppedIndex = *(const int*)payload->Data;
            if (droppedIndex != index) {
                if (scene.setParentChild(droppedIndex, index)) {
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

    // Add "Create Empty" button
    if (ImGui::Button("Create Empty")) {
        GameObject* emptyObj = new GameObject(nullptr, nullptr);
        scene.gameObjects.push_back(emptyObj);
        scene.gameObjectNames.push_back("Empty Object " + std::to_string(scene.gameObjects.size()));
    }

    ImGui::Text("Lista de GameObjects:");

    ImGui::BeginChild("GameObjectList", ImVec2(0, -30), true);

    for (size_t i = 0; i < scene.gameObjects.size(); ++i) {
        GameObject* obj = scene.gameObjects[i];
        if (!obj->getParent()) {
            drawGameObjectNode(obj, scene.gameObjectNames[i], i);
        }
    }
    ImGui::EndChild();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::BeginChild("DropZone", ImVec2(0, 25), true);
    ImGui::Text("Desvincular padre");

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DND")) {
            int droppedIndex = *(const int*)payload->Data;
            scene.removeFromParent(droppedIndex);
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::End();
}