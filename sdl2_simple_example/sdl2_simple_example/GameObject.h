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

    // Método para obtener la textura actual
    Texture* getTexture() const;

    // Método para establecer la transformación desde fuera de la clase
    void setTransform(const Transform& newTransform);

private:
    Mesh* mesh;
    Texture* texture;
    Transform transform;
};