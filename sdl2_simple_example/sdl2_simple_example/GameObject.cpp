#include "GameObject.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(Mesh* mesh, Texture* texture)
    : mesh(mesh), texture(texture) {}

GameObject::~GameObject() {
    delete mesh;
    if (texture) delete texture;
}

void GameObject::setTexture(Texture* newTexture) {
    texture = newTexture;
}


void GameObject::setTransform(const Transform& newTransform) {
    transform = newTransform;
}

void GameObject::draw() const {
    glPushMatrix(); // Guardar la matriz actual

    // Aplicar transformaciones en modo fijo
    glTranslatef(transform.position.x, transform.position.y, transform.position.z);
    glRotatef(transform.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(transform.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(transform.rotation.z, 0.0f, 0.0f, 1.0f);
    glScalef(transform.scale.x, transform.scale.y, transform.scale.z);

    if (texture) texture->bind(); // Vincular la textura si está disponible
    if (mesh) mesh->render();     // Renderizar la malla
    if (texture) texture->unbind();

    glPopMatrix(); // Restaurar la matriz anterior
}