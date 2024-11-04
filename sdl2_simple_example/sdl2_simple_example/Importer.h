#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "MyWindow.h"

using namespace std;

class Importer {
public:
    Importer();
    ~Importer();

    bool loadFBX(const string& filePath);
    GLuint loadTexture(const string& texturePath);

    const vector<float>& getVertices() const { return vertices; }
    const vector<float>& getUVs() const { return uvs; }
    const vector<unsigned int>& getIndices() const { return indices; }
    GLuint getTextureID() { return textureID; }

    GLuint createCheckerTexture();
    GLuint getTextureID() const;

    void setWindow(MyWindow* window);

private:
    vector<float> vertices;
    vector<float> uvs;
    vector<unsigned int> indices;
    GLuint textureID = 0;

    MyWindow* _window = nullptr;
};