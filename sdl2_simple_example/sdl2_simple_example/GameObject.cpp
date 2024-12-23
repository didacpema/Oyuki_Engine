#include "GameObject.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(Mesh* mesh, TextureData* texture)
    : mesh(mesh), texture(texture) {}

GameObject::~GameObject() {
    delete mesh;
    if (texture) delete texture;
}

void GameObject::setTexture(TextureData* newTexture) {
    texture = newTexture;
}


void GameObject::setTransform(const Transform& newTransform) {
    transform = newTransform;
}

void GameObject::updateAABB() {
    if (!mesh || mesh->vertices.empty()) {
        boundingBox = AABB();
        return;
    }

    // Initialize min and max with first vertex
    glm::vec3 min(mesh->vertices[0], mesh->vertices[1], mesh->vertices[2]);
    glm::vec3 max = min;

    // Find min/max vertices
    for (size_t i = 3; i < mesh->vertices.size(); i += 3) {
        glm::vec3 vertex(mesh->vertices[i], mesh->vertices[i + 1], mesh->vertices[i + 2]);

        min.x = std::min(min.x, vertex.x);
        min.y = std::min(min.y, vertex.y);
        min.z = std::min(min.z, vertex.z);

        max.x = std::max(max.x, vertex.x);
        max.y = std::max(max.y, vertex.y);
        max.z = std::max(max.z, vertex.z);
    }

    // Transform the bounding box by the object's transform
    min = min * transform.scale + transform.position;
    max = max * transform.scale + transform.position;

    boundingBox = AABB(min, max);
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