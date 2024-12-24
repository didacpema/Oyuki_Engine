#include "Inspector.h"

extern Scene scene;

Inspector::Inspector() {}
Inspector::~Inspector() {}

static TextureData* originalTexture = nullptr;

void Inspector::draw() {
    ImGui::Begin("Inspector");

    if (scene.selectedGameObjectIndex >= 0 && scene.selectedGameObjectIndex < scene.gameObjects.size()) {
        GameObject* selectedObject = scene.gameObjects[scene.selectedGameObjectIndex];
        if (!selectedObject) {
            ImGui::End();
            return;
        }

        ImGui::Text("Texture");

        // Safe texture handling
        if (selectedObject->texture) {
            if (selectedObject->texture->id != 2 && !originalTexture) {
                originalTexture = selectedObject->texture;
            }

            if (ImGui::Button("Off")) {
                if (!chekerOn && scene.checkerTextureID != 0) {
                    selectedObject->setTexture(new TextureData(scene.checkerTextureID));
                    chekerOn = true;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("On")) {
                if (chekerOn && originalTexture) {
                    selectedObject->setTexture(originalTexture);
                    chekerOn = false;
                }
            }

            // Display texture information
            GLuint textureID = selectedObject->texture->getTextureID();
            ImGui::Text("Texture ID: %u", textureID);

            if (selectedObject->texture->getHeight() && selectedObject->texture->getWidth()) {
                GLuint GetHeight = selectedObject->texture->getHeight();
                GLuint GetWidth = selectedObject->texture->getWidth();
                ImGui::Text("Texture Shape: %u x %u", GetHeight, GetWidth);
            }
            else {
                ImGui::Text("Texture Shape: None");
            }
            ImGui::Text("Texture path: %s", selectedObject->texture->getPath().c_str());
        }
        else {
            ImGui::Text("No texture assigned");
        }

        // Transform controls
        float position[3] = { selectedObject->transform.position.x, selectedObject->transform.position.y, selectedObject->transform.position.z };
        if (ImGui::InputFloat3("Posicion", position)) {
            selectedObject->transform.position = { position[0], position[1], position[2] };
        }

        float rotation[3] = { selectedObject->transform.rotation.x, selectedObject->transform.rotation.y, selectedObject->transform.rotation.z };
        if (ImGui::InputFloat3("Rotacion", rotation)) {
            selectedObject->transform.rotation = { rotation[0], rotation[1], rotation[2] };
        }

        float scale[3] = { selectedObject->transform.scale.x, selectedObject->transform.scale.y, selectedObject->transform.scale.z };
        if (ImGui::InputFloat3("Escala", scale)) {
            selectedObject->transform.scale = { scale[0], scale[1], scale[2] };
        }

        // Mesh information
        if (Mesh* mesh = selectedObject->getMesh()) {
            const std::vector<float>& vertices = mesh->getVertices();
            const std::vector<float>& uvs = mesh->getUVs();
            const std::vector<unsigned int>& indices = mesh->getIndices();

            ImGui::Text("Informacion de la Malla:");
            ImGui::Text("Vertices: %zu", vertices.size());
            ImGui::Text("UVs: %zu", uvs.size());
            ImGui::Text("Indices: %zu", indices.size());

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