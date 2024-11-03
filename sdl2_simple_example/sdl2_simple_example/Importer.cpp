#include "Importer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <iostream>
#include <fstream>

using vec3 = glm::dvec3;

float modelScale = 1.0f; // Global scaling factor for the model
vec3 modelCenter(0.0f); // Model center for positioning

Importer::Importer() {
    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);
}

Importer::~Importer() {
    if (textureID) {
        glDeleteTextures(1, &textureID);
    }
}

void centerModel(const aiScene* scene) {
    aiVector3D min, max;
    min = max = scene->mMeshes[0]->mVertices[0];

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        for (unsigned int v = 0; v < scene->mMeshes[i]->mNumVertices; v++) {
            aiVector3D vertex = scene->mMeshes[i]->mVertices[v];
            min.x = min(min.x, vertex.x);
            min.y = min(min.y, vertex.y);
            min.z = min(min.z, vertex.z);
            max.x = max(max.x, vertex.x);
            max.y = max(max.y, vertex.y);
            max.z = max(max.z, vertex.z);
        }
    }
    modelCenter = vec3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);
    modelScale = 2.0f / glm::length(vec3(max.x - min.x, max.y - min.y, max.z - min.z));
    //modelScale = 2.0f / glm::length(vec3(max.x - min.x, max.y - min.y, max.z - min.z));
}

bool Importer::loadFBX(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

    if (!scene) {
        std::cerr << "Error loading FBX: " << importer.GetErrorString() << std::endl;
        return false;
    }

    centerModel(scene);

    vertices.clear();
    uvs.clear();
    indices.clear();

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D vertex = mesh->mVertices[v];
            vertices.push_back(vertex.x);
            vertices.push_back(vertex.y);
            vertices.push_back(vertex.z);

            if (mesh->HasTextureCoords(0)) {
                aiVector3D texCoord = mesh->mTextureCoords[0][v];
                uvs.push_back(texCoord.x);
                uvs.push_back(1.0f - texCoord.y);
            }
            else {
                uvs.push_back(0.0f);
                uvs.push_back(0.0f);
            }
        }
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }
    std::cerr << "FBX loaded"<< std::endl;

    return true;
}

GLuint Importer::loadTexture(const std::string& texturePath) {
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (!ilLoadImage((const wchar_t*)texturePath.c_str())) {
        ilDeleteImages(1, &imageID);
        std::cerr << "Failed to load texture from: " << texturePath << std::endl;
        return 0;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    ilDeleteImages(1, &imageID);

    std::cerr << "Texture loaded" << std::endl;
    return textureID;
} 