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

    TextureData* getTexture() const { return texture; }
    Mesh* getMesh() const { return mesh; }

    void setTransform(const Transform& newTransform);

    Mesh* mesh;
    TextureData* texture;
    Transform transform;
};