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

    // Method to set the transform from outside
    void setTransform(const Transform& newTransform);

private:
    Mesh* mesh;
    Texture* texture;
    Transform transform;
};