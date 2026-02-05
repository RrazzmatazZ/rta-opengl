#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <functional>
#include <vector>

class Model;
class Shader;
class Camera;
class Skybox;

struct RenderCommand {
    Model* model;
    glm::mat4 modelMatrix;
    std::function<void(Shader*)> uniformCallback;
    float distToCamera;
};

class Renderer {
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const Camera& camera, float aspectRatio);

    static void Submit(Model& model, const glm::mat4& modelMatrix, std::function<void(Shader*)> callback = nullptr);

    static void SetSkybox(Skybox& skybox);

    static void EndScene();

private:
    static void Flush();
};