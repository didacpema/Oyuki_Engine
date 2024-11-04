#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include "glm/glm.hpp"

class Importer {
public:
    Importer();
    ~Importer();

    bool loadFBX(const std::string& filePath);
    GLuint loadTexture(const std::string& texturePath);

    const std::vector<float>& getVertices() const { return vertices; }
    const std::vector<float>& getUVs() const { return uvs; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
    GLuint getTextureID() { return textureID; }

    GLuint createCheckerTexture();
    GLuint getTextureID() const;

private:
    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<unsigned int> indices;
    GLuint textureID = 0;
};