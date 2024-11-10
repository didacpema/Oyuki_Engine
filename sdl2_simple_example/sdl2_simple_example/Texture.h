#pragma once
#include <GL/glew.h>
#include <string>

class TextureData {
public:
    GLuint id;
    std::string path;
    int width;
    int height;

    TextureData(GLuint textureId = 0, const std::string& filePath = "", int texWidth = 0, int texHeight = 0)
        : id(textureId), path(filePath), width(texWidth), height(texHeight) {}

    void bind() const;
    void unbind() const;

    GLuint getTextureID() const { return id; }
    std::string getPath() const { return path; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};