#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "Shader.h"

class Skybox
{
public:
    unsigned int textureID;
    Shader* shader;

    Skybox(std::vector<std::string> faces, const char* vsPath, const char* fsPath);
    ~Skybox();

    void Draw(const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int VAO, VBO;
    void setupSkybox();
    unsigned int loadCubemap(std::vector<std::string> faces);
};
