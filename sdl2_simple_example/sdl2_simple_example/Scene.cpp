#include "Scene.h"
#include "Importer.h"
#include "FilesystemUtils.h"
#include <GL/glew.h>

extern Importer importer;

Scene::Scene() {}

Scene::~Scene() {
    for (GameObject* obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();
}

void Scene::loadModelData(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices, const std::string& name,  const Transform& transform) {
    Mesh* mesh = new Mesh(vertices, uvs, indices);
    GameObject* gameObject = new GameObject(mesh, nullptr);

    gameObject->setTransform(transform);

    gameObjects.push_back(gameObject);
    gameObjectNames.push_back(name);
}

void Scene::setTexture(TextureData* newTexture) {
    if (!gameObjects.empty()) {
        GameObject* lastObject = gameObjects.back();
        if (lastObject->getTexture() != nullptr) {
            delete lastObject->getTexture();
        }
        lastObject->setTexture(newTexture);
    }
}

void Scene::setCheckerTexture(GLuint checkerTextureID) {
    // Creamos una textura temporal con el checkerTextureID, un path vacío y tamaño predeterminado (0,0)
    TextureData* checkerTexture = new TextureData(checkerTextureID, "checker_texture_path", 0, 0);
    
    // Asignamos esta textura a través de setTexture
    setTexture(checkerTexture);
}

void Scene::drawScene() {

    for (const auto& obj : gameObjects) {
        glPushMatrix();  // Guardar la matriz actual

        obj->draw();  // Dibujar el objeto

        glPopMatrix();   // Restaurar la matriz previa
    }
}
void Scene::createCube(const char* filePath) {
    
    std::string fileName = "Cube";

    std::string path(filePath);
    std::string extension = path.substr(path.find_last_of('.') + 1);

    if (importer.loadFBX(filePath)) {
        loadModelData(importer.getVertices(), importer.getUVs(), importer.getIndices(), fileName);

        setCheckerTexture(checkerTextureID);
    }
}

void Scene::createSphere(const std::string& name, const Transform& transform) {
    
    const int sectors = 36;  // Divide la esfera en 36 partes (puedes cambiar este número para más resolución)
    const int stacks = 18;    // Divide la esfera en 18 partes (puedes cambiar este número)

    // Vértices y UVs para la esfera
    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<unsigned int> indices;

    // Generar la malla esférica (esto se hace con un algoritmo para crear una esfera)
    for (int i = 0; i <= stacks; ++i) {
        float stackStep = angle / stacks;
        float stackAngle = angle / 2 - i * stackStep;

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * (2 * angle / sectors);

            // Coordenadas esféricas convertidas a cartesianas
            float x = cos(stackAngle) * cos(sectorAngle);
            float y = cos(stackAngle) * sin(sectorAngle);
            float z = sin(stackAngle);

            // Añadir vértices
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Generar coordenadas UV
            uvs.push_back((float)j / sectors);
            uvs.push_back((float)i / stacks);
        }
    }

    // Generar índices para el dibujo de las caras
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = (i * (sectors + 1)) + j;
            int second = first + sectors + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    loadModelData(vertices, uvs, indices, name, transform);
    setCheckerTexture(checkerTextureID);
}