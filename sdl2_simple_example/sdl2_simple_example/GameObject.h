#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"
#include <vector>
#include "AABB.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
class GameObject {
public:
    GameObject(Mesh* mesh, TextureData* texture);
    ~GameObject();

    void draw() const;
    void setTexture(TextureData* texture);
    void setParent(GameObject* newParent);
    void addChild(GameObject* child);
    void removeChild(GameObject* child);

    Transform getGlobalTransform() const;
    void setTransform(const Transform& newTransform);
    bool isChildOf(GameObject* potentialParent) const;

    TextureData* getTexture() const { return texture; }
    Mesh* getMesh() const { return mesh; }
    GameObject* getParent() const { return parent; }
    const std::vector<GameObject*>& getChildren() const { return children; }

    Mesh* mesh;
    TextureData* texture;
    Transform transform;
    GameObject* parent;
    std::vector<GameObject*> children;

    bool isVisible = true;

    AABB getTransformedBoundingBox() const {
        AABB transformedBox = mesh->boundingBox;
        transformedBox.transform(transform);
        return transformedBox;
    }

    void drawBoundingBox() const {
        if (!mesh) return;

        AABB box = getTransformedBoundingBox();
        auto corners = box.getCorners();

        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 0.0f, 0.0f);  // Red color for bounding box
        glBegin(GL_LINES);

        // Draw front face
        glVertex3fv(glm::value_ptr(corners[0]));
        glVertex3fv(glm::value_ptr(corners[1]));

        glVertex3fv(glm::value_ptr(corners[1]));
        glVertex3fv(glm::value_ptr(corners[3]));

        glVertex3fv(glm::value_ptr(corners[3]));
        glVertex3fv(glm::value_ptr(corners[2]));

        glVertex3fv(glm::value_ptr(corners[2]));
        glVertex3fv(glm::value_ptr(corners[0]));

        // Draw back face
        glVertex3fv(glm::value_ptr(corners[4]));
        glVertex3fv(glm::value_ptr(corners[5]));

        glVertex3fv(glm::value_ptr(corners[5]));
        glVertex3fv(glm::value_ptr(corners[7]));

        glVertex3fv(glm::value_ptr(corners[7]));
        glVertex3fv(glm::value_ptr(corners[6]));

        glVertex3fv(glm::value_ptr(corners[6]));
        glVertex3fv(glm::value_ptr(corners[4]));

        // Draw connecting lines
        glVertex3fv(glm::value_ptr(corners[0]));
        glVertex3fv(glm::value_ptr(corners[4]));

        glVertex3fv(glm::value_ptr(corners[1]));
        glVertex3fv(glm::value_ptr(corners[5]));

        glVertex3fv(glm::value_ptr(corners[2]));
        glVertex3fv(glm::value_ptr(corners[6]));

        glVertex3fv(glm::value_ptr(corners[3]));
        glVertex3fv(glm::value_ptr(corners[7]));

        glEnd();
        glEnable(GL_TEXTURE_2D);
    }
};