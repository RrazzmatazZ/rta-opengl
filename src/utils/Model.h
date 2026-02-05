#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "RenderTypes.h"

#include <string>
#include <vector>

class Model
{
public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh>    meshes;
    std::string directory;
    bool gammaCorrection;

    Shader* modelShader;

    Model(std::string const &path, const char* vsPath, const char* fsPath, bool gamma = false);
    ~Model();

    void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

private:
    const aiScene* scene_ptr;

    void loadModel(std::string const &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    
    unsigned int TextureFromMemory(const aiTexture* aiTex);
    unsigned int TextureFromFile(const char *path, const std::string &directory);
};
#endif