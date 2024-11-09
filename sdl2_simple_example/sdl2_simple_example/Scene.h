#pragma once
#include <vector>
#include <string>
#include "GameObject.h"
#include <glm/glm.hpp>
#include <cmath>


class Scene {
public:
    Scene();
    ~Scene();
    void loadModelData(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices, const std::string& name, const Transform& transform = Transform());
    void createSphere(const std::string& name, const Transform& transform = Transform());
    void createCube(const std::string& name, const Transform& transform = Transform());
    void setTexture(GLuint textureID);
    void setCheckerTexture(GLuint checkerTextureID);
    void drawScene();

    std::vector<GameObject*> gameObjects;
    std::vector<std::string> gameObjectNames;

    // Índice del objeto seleccionado (-1 si no hay ninguno)
    int selectedGameObjectIndex = -1;
    float angle = 3.14159265359f;
	GLuint checkerTextureID = 0;
};