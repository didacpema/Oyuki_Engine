#include "GameObject.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

GameObject::GameObject(Mesh* mesh, TextureData* texture)
    : mesh(mesh), texture(texture), parent(nullptr) {}

GameObject::~GameObject() {
    // Desvinculamos todos los hijos antes de destruir el objeto
    for (auto* child : children) {
        child->parent = nullptr;
    }

    // Si tenemos un padre, nos removemos de su lista de hijos
    if (parent) {
        parent->removeChild(this);
    }
    delete mesh;
    if (texture) delete texture;
}

void GameObject::setTexture(TextureData* newTexture) {
    texture = newTexture;
}


void GameObject::setTransform(const Transform& newTransform) {
    transform = newTransform;
}
bool GameObject::isChildOf(GameObject* potentialParent) const {
    if (!potentialParent) return false;

    GameObject* current = parent;
    while (current) {
        if (current == potentialParent) return true;
        current = current->getParent();
    }
    return false;
}
void GameObject::setParent(GameObject* newParent) {
    // Si ya teníamos un padre, nos removemos de su lista de hijos
    if (parent) {
        parent->removeChild(this);
    }

    parent = newParent;

    // Nos agregamos a la lista de hijos del nuevo padre
    if (parent) {
        parent->addChild(this);
    }
}

void GameObject::addChild(GameObject* child) {
    if (child && std::find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
        if (child->parent != this) {
            child->setParent(this);
        }
    }
}

void GameObject::removeChild(GameObject* child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
        if (child->parent == this) {
            child->parent = nullptr;
        }
    }
}

Transform GameObject::getGlobalTransform() const {
    Transform globalTransform = transform;

    if (parent) {
        Transform parentTransform = parent->getGlobalTransform();

        // Aplicar transformaciones del padre
        globalTransform.position = parentTransform.position + transform.position;
        globalTransform.rotation = parentTransform.rotation + transform.rotation;
        globalTransform.scale.x *= parentTransform.scale.x;
        globalTransform.scale.y *= parentTransform.scale.y;
        globalTransform.scale.z *= parentTransform.scale.z;
    }

    return globalTransform;
}

void GameObject::draw() const {
    glPushMatrix();  // Save the current matrix

    Transform globalTransform = getGlobalTransform();
    // Apply transformations based on the GameObject's transform
    glTranslatef(globalTransform.position.x, globalTransform.position.y, globalTransform.position.z);
    glRotatef(globalTransform.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(globalTransform.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(globalTransform.rotation.z, 0.0f, 0.0f, 1.0f);
    glScalef(globalTransform.scale.x, globalTransform.scale.y, globalTransform.scale.z);

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
    // Dibujar todos los hijos
    for (const auto* child : children) {
        child->draw();
    }
    glPopMatrix();  // Restore the previous matrix
}