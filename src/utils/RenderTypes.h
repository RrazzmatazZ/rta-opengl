#pragma once

#include <glm/glm.hpp>
#include <string>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[4];
    float m_Weights[4];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};