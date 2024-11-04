#pragma once
#include <vector>
#include "GameObject.h"
#include <string>
using namespace std;
class Scene {
public:
    Scene();
    ~Scene();
    void loadModelData(const vector<float>& vertices, const vector<float>& uvs, const vector<unsigned int>& indices, const string& name, const Transform& transform = Transform());
    void setTexture(GLuint textureID);
    void setCheckerTexture(GLuint checkerTextureID);
    void drawScene();
    vector<string> gameObjectNames;
    vector<GameObject*> gameObjects;
    
   
};