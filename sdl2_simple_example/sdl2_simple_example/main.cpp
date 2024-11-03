#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "stdio.h"
#include "assimp/cimport.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <IL/il.h>       // DevIL for image loading
#include <IL/ilu.h>      // Additional DevIL functions
#include <IL/ilut.h>     // For OpenGL integration with DevIL
#include <string>        // For string handling

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#include "queue"

#define CHECKERS_WIDTH 64
#define CHECKERS_HEIGHT 64

GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
GLuint textureID;

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

vector<float> vertices;
vector<float> uvs;
vector<unsigned int> indices;

float modelScale = 1.0f; // Global scaling factor for the model
vec3 modelCenter(0.0f); // Model center for positioning

MyWindow myWindow("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);

void setupProjection(float fov, float aspectRatio, float nearPlane, float farPlane) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspectRatio, nearPlane, farPlane);
}

void setupView() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float eyeX = myWindow.cameraDistance * sin(myWindow.cameraAngleY) * cos(myWindow.cameraAngleX) + myWindow.panX;
    float eyeY = myWindow.cameraDistance * sin(myWindow.cameraAngleX) + myWindow.panY;
    float eyeZ = myWindow.cameraDistance * cos(myWindow.cameraAngleY) * cos(myWindow.cameraAngleX);

    gluLookAt(eyeX, eyeY, eyeZ, myWindow.panX, myWindow.panY, 0.0, 0.0, 1.0, 0.0);
}

static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw std::exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);

    float fov = 45.0f;
    float aspectRatio = static_cast<float>(WINDOW_SIZE.x) / static_cast<float>(WINDOW_SIZE.y);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    setupProjection(fov, aspectRatio, nearPlane, farPlane);
}

void generateCheckerImage() {
    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++) {
            int c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
            checkerImage[i][j][0] = (GLubyte)c;
            checkerImage[i][j][1] = (GLubyte)c;
            checkerImage[i][j][2] = (GLubyte)c;
            checkerImage[i][j][3] = (GLubyte)255;
        }
    }
}

GLuint loadTexture(const char* texturePath) {
    ILuint imageID;
    GLuint textureID;

    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (!ilLoadImage((const wchar_t*)texturePath)) {
        ilDeleteImages(1, &imageID);
        fprintf(stderr, "Failed to load texture from: %s\n", texturePath);
        return 0;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
        ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

    ilDeleteImages(1, &imageID);
    return textureID;
}

void drawTextures(const char* texturePath) {
    textureID = loadTexture(texturePath);

    if (textureID == 0) {
        fprintf(stderr, "Texture loading failed. Using default checkerboard texture.\n");
        generateCheckerImage();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
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
    //modelScale = 2.0f / glm::length(vec3(max.x - min.x, max.y - min.y, max.z - min.z));
}

bool loadFBX(const char* filePath) {
    const struct aiScene* scene = aiImportFile(filePath, aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

    if (!scene) {
        fprintf(stderr, "Error loading file: %s\n", aiGetErrorString());
        return false;
    }

    printf("FBX Loaded with %d meshes\n", scene->mNumMeshes);
    centerModel(scene);

    vertices.clear();
    uvs.clear();
    indices.clear();

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        printf("Mesh %d has %d vertices\n", i, mesh->mNumVertices);

        // Collect vertices and uvs
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

        // Collect indices
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    aiReleaseImport(scene);
    printf("Model loading complete. Vertex count: %lu, UV count: %lu, Index count: %lu\n", vertices.size(), uvs.size(), indices.size());
    return true;
}

void drawFBX() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPushMatrix();
    glTranslatef(-modelCenter.x, -modelCenter.y, -modelCenter.z);
    glScalef(modelScale, modelScale, modelScale);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices.data());
    glTexCoordPointer(2, GL_FLOAT, 0, uvs.data());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setupView();
    drawFBX();
    glFlush();
}

// Function to handle dropped files and determine their type
void handleFileDrop(const char* filePath) {
    std::string path(filePath);
    std::string extension = path.substr(path.find_last_of('.') + 1);

    if (extension == "fbx") {
        if (!loadFBX(filePath)) {
            fprintf(stderr, "Failed to load FBX model.\n");
        }
    }
    else if (extension == "png" || extension == "jpg" || extension == "jpeg") {
        GLuint newTexture = loadTexture(filePath);
        if (newTexture != 0) {
            glDeleteTextures(1, &textureID);
            textureID = newTexture;
        }
        else {
            fprintf(stderr, "Failed to load texture.\n");
        }
    }
    else {
        fprintf(stderr, "Unsupported file type: %s\n", filePath);
    }
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);
    init_openGL();

    //loadFBX("C:/Users/User/Desktop/BakerHouse.fbx");
    //drawTextures("C:/Users/User/Desktop/Baker_house.png");

    while (myWindow.processEvents() && myWindow.isOpen()) {
        const auto t0 = hrclock::now();
        display_func();
        myWindow.draw();
        myWindow.swapBuffers(); // Swap buffers to display
        //gui.render();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    }
    return 0;
}