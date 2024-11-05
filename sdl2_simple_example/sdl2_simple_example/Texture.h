#pragma once
#include <GL/glew.h>

class Texture {
public:
    GLuint id;

    Texture(GLuint textureId = 0) : id(textureId) {}

    void bind() const;
    void unbind() const;

    // Getter for texture ID
    GLuint getTextureID() const {return id;}
};