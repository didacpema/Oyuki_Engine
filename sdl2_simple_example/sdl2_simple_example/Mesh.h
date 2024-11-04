#pragma once
#include <vector>
#include <GL/glew.h>
using namespace std;
class Mesh {
public:
    vector<float> vertices;
    vector<float> uvs;
    vector<unsigned int> indices;
    GLuint vao, vboVertices, vboUVs, ebo;

    Mesh(const vector<float>& vertices, const vector<float>& uvs, const vector<unsigned int>& indices);
    ~Mesh();
    void setupMesh();
    void render() const;
};