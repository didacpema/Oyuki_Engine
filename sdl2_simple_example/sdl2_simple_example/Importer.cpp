#include "Importer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <iostream>
#include <fstream>
#include "MyWindow.h"

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

void Importer::setWindow(MyWindow* window) {
    _window = window;
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

// Dentro de la clase Importer, agrega un método para crear una textura checker
GLuint Importer::createCheckerTexture() {
    const int width = 64;
    const int height = 64;
    vector<GLubyte> checkerImage(width * height * 4);

    // Generar el patrón checker
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int c = ((x / 8) % 2) == ((y / 8) % 2) ? 255 : 0;
            checkerImage[(y * width + x) * 4 + 0] = (GLubyte)c;
            checkerImage[(y * width + x) * 4 + 1] = (GLubyte)c;
            checkerImage[(y * width + x) * 4 + 2] = (GLubyte)c;
            checkerImage[(y * width + x) * 4 + 3] = 255;  // Alpha
        }
    }

    GLuint checkerTexture;
    glGenTextures(1, &checkerTexture);
    glBindTexture(GL_TEXTURE_2D, checkerTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return checkerTexture;
}

bool Importer::loadFBX(const string& filePath) {
    _window->logMessage("Attempting to load FBX file: " + filePath);  // New log message

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

    if (!scene) {
        _window->logMessage("Error loading FBX: " + string(importer.GetErrorString()));
        return false;
    }

    _window->logMessage("Successfully loaded FBX file: " + filePath);

    // Centering the model and logging its details
    centerModel(scene);
    _window->logMessage("Model centered with scale factor: " + to_string(modelScale) + ", center: (" +
        to_string(modelCenter.x) + ", " +
       to_string(modelCenter.y) + ", " +
        to_string(modelCenter.z) + ")");

    vertices.clear();
    uvs.clear();
    indices.clear();

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        _window->logMessage("Processing mesh: " + to_string(i) + " with " +
            to_string(mesh->mNumVertices) + " vertices and " +
            to_string(mesh->mNumFaces) + " faces.");

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

    if (scene->mNumMaterials == 0) {
        textureID = createCheckerTexture();
        _window->logMessage("No texture found in model. Checker texture applied.");
    }
    else {
        _window->logMessage("Texture detected and applied.");
    }

    return true;
}

GLuint Importer::loadTexture(const string& texturePath) {
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (!ilLoadImage((const wchar_t*)texturePath.c_str())) {
        ilDeleteImages(1, &imageID);
        cerr << "Failed to load texture from: " << texturePath << endl;
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

    cerr << "Texture loaded" << endl;
    return textureID;
}

// Método para obtener la textura cargada o checker
GLuint Importer::getTextureID() const {
    return textureID;
}