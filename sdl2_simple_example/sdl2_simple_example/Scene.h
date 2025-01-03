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
    void createCube(const char* filePath);
    void setTexture(TextureData* newTexture);
    void setCheckerTexture(GLuint checkerTextureID);
    void drawScene();
    
    // Nuevos m�todos para manejar la jerarqu�a
    bool setParentChild(int childIndex, int parentIndex);
    void removeFromParent(int childIndex);
    bool isValidParenting(int childIndex, int parentIndex) const;

    std::vector<GameObject*> gameObjects;
    std::vector<std::string> gameObjectNames;

    // �ndice del objeto seleccionado (-1 si no hay ninguno)
    int selectedGameObjectIndex = -1;
    float angle = 3.14159265359f;
	GLuint checkerTextureID;

    void deleteGameObjectHierarchy(GameObject* obj);

};