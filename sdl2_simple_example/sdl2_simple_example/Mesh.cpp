#include "Mesh.h"
using namespace std;
Mesh::Mesh(const vector<float>& vertices, const vector<float>& uvs, const vector<unsigned int>& indices)
    : vertices(vertices), uvs(uvs), indices(indices) {
    setupMesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboUVs);
    glDeleteBuffers(1, &ebo);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboVertices);
    glGenBuffers(1, &vboUVs);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vboUVs);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::render() const {
    glBindVertexArray(vao);

    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);

    if (!uvs.empty()) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, vboUVs);
        glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    glDisableClientState(GL_VERTEX_ARRAY);
    if (!uvs.empty()) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glBindVertexArray(0);
}