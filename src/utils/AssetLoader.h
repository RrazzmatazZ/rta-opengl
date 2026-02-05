#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>

#include "Mesh.h"
#include "RenderTypes.h"

// 简单的资源加载器，封装 Assimp
class AssetLoader {
public:
    static std::vector<Mesh> LoadModel(const std::string& path) {
        Assimp::Importer importer;
        // 常用后期处理：三角化、生成法线、翻转UV、计算切线空间
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        std::vector<Mesh> meshes;

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return meshes;
        }

        ProcessNode(scene->mRootNode, scene, meshes);
        return meshes;
    }

private:
    static void ProcessNode(aiNode *node, const aiScene *scene, std::vector<Mesh>& meshes) {
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene));
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene, meshes);
        }
    }

    static Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<TextureInfo> textures;

        // 1. 处理顶点
        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            // 位置
            vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            
            // 法线
            if(mesh->HasNormals())
                vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
            
            // UV
            if(mesh->mTextureCoords[0]) {
                vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
            } else {
                vertex.TexCoords = {0.0f, 0.0f};
            }

            // 切线与副切线 (用于法线贴图)
            if (mesh->HasTangentsAndBitangents()) {
                vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                vertex.Bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
            }

            vertices.push_back(vertex);
        }

        // 2. 处理索引
        for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // 3. 处理纹理 (简化版：只记录材质名称或路径，实际加载交给 Renderer 懒加载)
        if(mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // 这里可以添加逻辑提取漫反射贴图路径等
            // 为了代码简洁，暂时留空，你可以根据 Model.cpp 把 loadMaterialTextures 逻辑移过来
        }

        return Mesh(vertices, indices, textures);
    }
};