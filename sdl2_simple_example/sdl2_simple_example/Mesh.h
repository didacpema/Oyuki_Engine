#pragma once
#include <vector>
#include <GL/glew.h>

class Mesh {
public:
    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<unsigned int> indices;
    GLuint vao, vboVertices, vboUVs, ebo;

    Mesh(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices);
    ~Mesh();

    void setupMesh();
    void render() const;

    // Getter methods for mesh information
    const std::vector<float>& getVertices() const { return vertices; }
    const std::vector<float>& getUVs() const { return uvs; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
};