#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"
#include <vector>

class GameObject {
public:
    GameObject(Mesh* mesh, TextureData* texture);
    ~GameObject();

    void draw() const;
    void setTexture(TextureData* texture);
    void setParent(GameObject* newParent);
    void addChild(GameObject* child);
    void removeChild(GameObject* child);

    Transform getGlobalTransform() const;
    void setTransform(const Transform& newTransform);
    bool isChildOf(GameObject* potentialParent) const;

    TextureData* getTexture() const { return texture; }
    Mesh* getMesh() const { return mesh; }
    GameObject* getParent() const { return parent; }
    const std::vector<GameObject*>& getChildren() const { return children; }

    Mesh* mesh;
    TextureData* texture;
    Transform transform;
    GameObject* parent;
    std::vector<GameObject*> children;
};