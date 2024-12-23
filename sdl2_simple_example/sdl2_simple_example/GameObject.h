#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"
#include "AABB.h"

class GameObject {
public:
    GameObject(Mesh* mesh, TextureData* texture);
    ~GameObject();

    void draw() const;
    void setTexture(TextureData* texture);

    TextureData* getTexture() const { return texture; }
    Mesh* getMesh() const { return mesh; }

    void setTransform(const Transform& newTransform);

    const AABB& getAABB() const { return boundingBox; }
    void updateAABB();

    Mesh* mesh;
    TextureData* texture;
    Transform transform;
    AABB boundingBox;
};