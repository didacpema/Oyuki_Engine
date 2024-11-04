#pragma once
#include <vector>
#include <string>
#include "GameObject.h"

class Scene {
public:
    Scene();
    ~Scene();
    void loadModelData(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices, const std::string& name, const Transform& transform = Transform());
    void setTexture(GLuint textureID);
    void setCheckerTexture(GLuint checkerTextureID);
    void drawScene();
    std::vector<GameObject*> gameObjects;
    std::vector<std::string> gameObjectNames;
};