#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"

class GameObject {
public:
    GameObject(Mesh* mesh, Texture* texture);
    ~GameObject();

    void draw() const;
    void setTexture(Texture* texture);

    Texture* getTexture() const { return texture; }
    Mesh* getMesh() const { return mesh; }

    void setTransform(const Transform& newTransform);

    Mesh* mesh;
    Texture* texture;
    Transform transform;
};