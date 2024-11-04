#include "Scene.h"
#include <GL/glew.h>

Scene::Scene() {}

Scene::~Scene() {
    for (GameObject* obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();
}

void Scene::loadModelData(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices, const Transform& transform) {
    Mesh* mesh = new Mesh(vertices, uvs, indices);
    GameObject* gameObject = new GameObject(mesh, nullptr);

    gameObject->setTransform(transform);

    gameObjects.push_back(gameObject);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& obj : gameObjects) {
        glPushMatrix();  // Guardar la matriz actual

        obj->draw();  // Dibujar el objeto

        glPopMatrix();   // Restaurar la matriz previa
    }
}