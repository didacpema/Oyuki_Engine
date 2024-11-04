#pragma once
#include <vector>
#include <string>
#include <fstream>
using namespace std;
void saveCustomFormat(const string& path, const vector<float>& vertices, const vector<float>& uvs, const vector<unsigned int>& indices) {
    ofstream file(path, ios::binary);

    size_t vertexCount = vertices.size();
    size_t uvCount = uvs.size();
    size_t indexCount = indices.size();

    file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(vertices.data()), vertices.size() * sizeof(float));

    file.write(reinterpret_cast<const char*>(&uvCount), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(uvs.data()), uvs.size() * sizeof(float));

    file.write(reinterpret_cast<const char*>(&indexCount), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(indices.data()), indices.size() * sizeof(unsigned int));

    file.close();
}

void loadCustomFormat(const string& path, vector<float>& vertices, vector<float>& uvs, vector<unsigned int>& indices) {
    ifstream file(path, ios::binary);

    size_t vertexCount, uvCount, indexCount;
    file.read(reinterpret_cast<char*>(&vertexCount), sizeof(size_t));
    vertices.resize(vertexCount);
    file.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(float));

    file.read(reinterpret_cast<char*>(&uvCount), sizeof(size_t));
    uvs.resize(uvCount);
    file.read(reinterpret_cast<char*>(uvs.data()), uvCount * sizeof(float));

    file.read(reinterpret_cast<char*>(&indexCount), sizeof(size_t));
    indices.resize(indexCount);
    file.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(unsigned int));

    file.close();
}