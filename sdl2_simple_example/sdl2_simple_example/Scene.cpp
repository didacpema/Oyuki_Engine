#include "Scene.h"
#include <GL/glew.h>

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

void Scene::setTexture(GLuint textureID) {
    if (!gameObjects.empty()) {
        Texture* newTexture = new Texture(textureID);

        GameObject* lastObject = gameObjects.back();
        if (lastObject->getTexture() != nullptr) {
            delete lastObject->getTexture();
        }
        lastObject->setTexture(newTexture);
    }
}

void Scene::setCheckerTexture(GLuint checkerTextureID) {
    setTexture(checkerTextureID);
}

void Scene::drawScene() {

    for (const auto& obj : gameObjects) {
        glPushMatrix();  // Guardar la matriz actual

        obj->draw();  // Dibujar el objeto

        glPopMatrix();   // Restaurar la matriz previa
    }
}
void Scene::createCube(const std::string& name, const Transform& transform) {
    // Datos básicos del cubo (vértices, UVs, índices)
    std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f
    };

    std::vector<float> uvs = {
        0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
        0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,  2, 3, 0,  4, 5, 6,  6, 7, 4,
        0, 1, 5,  5, 4, 0,  2, 3, 7,  7, 6, 2,
        0, 3, 7,  7, 4, 0,  1, 2, 6,  6, 5, 1
    };

    loadModelData(vertices, uvs, indices, name, transform); // Añadir el cubo a la escena
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
}