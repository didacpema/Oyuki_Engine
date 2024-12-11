#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"

class GameObject {
public:
    GameObject(Mesh* mesh, TextureData* texture);
    ~GameObject();

    void draw() const;
    void setTexture(TextureData* texture);

    // Métodos de jerarquía
    void addChild(GameObject* child);
    void removeChild(GameObject* child);
    const std::vector<GameObject*>& getChildren() const;

    bool isDescendantOf(GameObject* other) const;

    // Acceso al nombre
    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    TextureData* getTexture() const { return texture; }
    Mesh* getMesh() const { return mesh; }
    void setTransform(const Transform& newTransform);


    std::string name; // Nombre del objeto
    std::vector<GameObject*> children; // Lista de hijos
    Mesh* mesh;
    TextureData* texture;
    Transform transform;
};