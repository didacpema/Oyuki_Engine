#include "GameObject.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(Mesh* mesh, TextureData* texture)
    : mesh(mesh), texture(texture) {}

GameObject::~GameObject() {
    delete mesh;
    if (texture) delete texture;

    // Elimina todos los hijos
    for (auto child : children) {
        delete child;
    }
}

void GameObject::addChild(GameObject* child) {
    if (child) {
        children.push_back(child);
    }
}

void GameObject::removeChild(GameObject* child) {
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

const std::vector<GameObject*>& GameObject::getChildren() const {
    return children;
}
bool GameObject::isDescendantOf(GameObject* other) const {
    GameObject* currentParent = this->parent;  // Asume que tienes un puntero a un padre

    // Recorre la cadena de padres hasta encontrar el objeto 'other'
    while (currentParent != nullptr) {
        if (currentParent == other) {
            return true;  // Si el objeto actual es un descendiente del 'other'
        }
        currentParent = currentParent->parent;  // Subimos al siguiente nivel
    }

    return false;  // No encontramos al objeto 'other' en la cadena de padres
}
void GameObject::setTexture(TextureData* newTexture) {
    texture = newTexture;
}


void GameObject::setTransform(const Transform& newTransform) {
    transform = newTransform;
}

void GameObject::draw() const {
    glPushMatrix();  // Save the current matrix

    // Apply transformations based on the GameObject's transform
    glTranslatef(transform.position.x, transform.position.y, transform.position.z);
    glRotatef(transform.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(transform.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(transform.rotation.z, 0.0f, 0.0f, 1.0f);
    glScalef(transform.scale.x, transform.scale.y, transform.scale.z);

    // Enable textures if there is a texture bound
    if (texture) {
        glEnable(GL_TEXTURE_2D);
        texture->bind();  // Bind the object's texture
    }
    else {
        glDisable(GL_TEXTURE_2D);  // Disable if no texture
    }

    if (mesh) mesh->render();  // Render the mesh

    // Unbind and disable texture after rendering to prevent interference with other objects
    if (texture) {
        texture->unbind();
        glDisable(GL_TEXTURE_2D);
    }

    for (const auto& child : children) {
        child->draw();
    }

    glPopMatrix();  // Restore the previous matrix
}