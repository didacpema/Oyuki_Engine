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

    glPopMatrix();  // Restore the previous matrix
}