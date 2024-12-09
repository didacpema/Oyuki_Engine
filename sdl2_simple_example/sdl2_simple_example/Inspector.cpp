#include "Inspector.h"

extern Scene scene;

Inspector::Inspector() {}
Inspector::~Inspector(){}

static TextureData* originalTexture;
void Inspector::draw() {


    // Inspector (vacio)
    ImGui::Begin("Inspector");

    if (scene.selectedGameObjectIndex >= 0 && scene.selectedGameObjectIndex < scene.gameObjects.size()) {
        GameObject* selectedObject = scene.gameObjects[scene.selectedGameObjectIndex];

        ImGui::Text("Texture");
        // Guardar la textura original solo si aún no lo hemos hecho
        if (selectedObject->texture->id != 2)
        {
            originalTexture = selectedObject->texture;
        }

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