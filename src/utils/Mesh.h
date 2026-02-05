#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Shader.h"
#include "RenderTypes.h"

class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    // 渲染网格
    void Draw(Shader &shader);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};
#endif