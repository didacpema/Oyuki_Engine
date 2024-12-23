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
    glm::mat4 localMatrix = glm::mat4(1.0f);
    localMatrix = glm::translate(localMatrix, glm::vec3(transform.position.x, transform.position.y, transform.position.z));
    localMatrix = glm::rotate(localMatrix, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    localMatrix = glm::rotate(localMatrix, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    localMatrix = glm::rotate(localMatrix, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    localMatrix = glm::scale(localMatrix, glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z));

    // Multiply with parent's matrix if exists
    if (parent) {
        glm::mat4 parentMatrix = glm::mat4(1.0f);
        Transform parentTransform = parent->getGlobalTransform();
        parentMatrix = glm::translate(parentMatrix, glm::vec3(parentTransform.position.x, parentTransform.position.y, parentTransform.position.z));
        parentMatrix = glm::rotate(parentMatrix, glm::radians(parentTransform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        parentMatrix = glm::rotate(parentMatrix, glm::radians(parentTransform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        parentMatrix = glm::rotate(parentMatrix, glm::radians(parentTransform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        parentMatrix = glm::scale(parentMatrix, glm::vec3(parentTransform.scale.x, parentTransform.scale.y, parentTransform.scale.z));

        localMatrix = parentMatrix * localMatrix;
    }

    Transform globalTransform;

    // Extract position
    glm::vec3 position = glm::vec3(localMatrix[3]);
    globalTransform.position.x = position.x;
    globalTransform.position.y = position.y;
    globalTransform.position.z = position.z;

    // Extract rotation
    glm::vec3 rotation;
    rotation.x = glm::degrees(atan2(localMatrix[1][2], localMatrix[2][2]));
    rotation.y = glm::degrees(asin(-localMatrix[0][2]));
    rotation.z = glm::degrees(atan2(localMatrix[0][1], localMatrix[0][0]));
    globalTransform.rotation.x = rotation.x;
    globalTransform.rotation.y = rotation.y;
    globalTransform.rotation.z = rotation.z;

    // Extract scale
    globalTransform.scale.x = glm::length(glm::vec3(localMatrix[0]));
    globalTransform.scale.y = glm::length(glm::vec3(localMatrix[1]));
    globalTransform.scale.z = glm::length(glm::vec3(localMatrix[2]));

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