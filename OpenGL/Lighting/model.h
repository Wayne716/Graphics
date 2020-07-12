#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "stb_image.h"
#include "shader.h"
#include "mesh.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false) {
    string filename = string(path);
    filename = directory + '/' + filename;
    
    unsigned int tex;
    glGenTextures(1, &tex);
    
    int width, height, colorChannel;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &colorChannel, 0);
    if (data) {
        GLenum format = (colorChannel == 3)? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else {
        std::cout << "FAILED LOADING IMAGE" << std::endl;
        stbi_image_free(data);
    }
    
    return tex;
}

class Model {
public:
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;
    
public:
    Model(const string& path, bool gamma = false) : gammaCorrection(gamma) {
        loadModel(path);
    }
    
    void Draw(Shader& shader) {
        for (unsigned int i=0; i<meshes.size(); ++i)
            meshes[i].Draw(shader);
    }
    
private:
    void loadModel(const string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || (scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            cout << importer.GetErrorString() << endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }
    
    void processNode(aiNode* node, const aiScene* scene) {
        // 如果节点包含网格处理网络
        for (unsigned int i=0; i<node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i=0; i<node->mNumChildren; ++i)
            processNode(node->mChildren[i], scene);
    }
    
    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
                
        for (unsigned int i=0; i<mesh->mNumVertices; ++i) {
            Vertex vertex;
            glm::vec3 v3;
            
            v3.x = mesh->mVertices[i].x;
            v3.y = mesh->mVertices[i].y;
            v3.z = mesh->mVertices[i].z;
            vertex.Position = v3;

            v3.x = mesh->mNormals[i].x;
            v3.y = mesh->mNormals[i].y;
            v3.z = mesh->mNormals[i].z;
            vertex.Normal = v3;
            
            if (mesh->mTextureCoords[0]) {
                glm::vec2 v2;
                v2.x = mesh->mTextureCoords[0][i].x;
                v2.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = v2;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            
            v3.x = mesh->mTangents[i].x;
            v3.y = mesh->mTangents[i].y;
            v3.z = mesh->mTangents[i].z;
            vertex.Tangent = v3;
            
            v3.x = mesh->mBitangents[i].x;
            v3.y = mesh->mBitangents[i].y;
            v3.z = mesh->mBitangents[i].z;
            vertex.Bitangent = v3;
            
            vertices.push_back(vertex);
        }
        
        for (unsigned int i=0; i<mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j=0; j<face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }
        
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // 材质对象的内部对每种纹理类型都存储了一个纹理位置数组
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        
        return Mesh(vertices, indices, textures);
    }
    
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
        vector<Texture> textures;
        for (unsigned int i=0; i<mat->GetTextureCount(type); ++i) {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j=0; j<textures_loaded.size(); ++j) {
                if (strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                Texture tex;
                tex.ID = TextureFromFile(str.C_Str(), directory);
                tex.type = typeName;
                tex.path = str.C_Str();
                textures.push_back(tex);
                textures_loaded.push_back(tex);
            }
        }
        return textures;
    }
};
