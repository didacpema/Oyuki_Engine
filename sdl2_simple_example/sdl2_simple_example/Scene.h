#pragma once
#include <vector>
#include "GameObject.h"

class Scene {
public:
    Scene();
    ~Scene();
    void loadModelData(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices, const Transform& transform = Transform());
    void setTexture(GLuint textureID);
    void setCheckerTexture(GLuint checkerTextureID);
    void drawScene();

private:
    std::vector<GameObject*> gameObjects;
};