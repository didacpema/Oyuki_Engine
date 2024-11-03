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
        Texture* texture = new Texture(textureID);
        gameObjects.back()->setTexture(texture);
    }
}

void Scene::drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& obj : gameObjects) {
        // Apply transformations for each object independently
        glPushMatrix();  // Save the current matrix

        // Apply the object's own transformations
        obj->draw();

        glPopMatrix();   // Restore the previous matrix
    }
}