#include "Renderer.h"
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include "Skybox.h"
#include <algorithm>

struct RendererData
{
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPosition;
    std::vector<RenderCommand> commandQueue;

    Skybox* activeSkybox = nullptr;
};

static RendererData s_Data;

void Renderer::Init()
{
    glEnable(GL_DEPTH_TEST);
}

void Renderer::Shutdown()
{
    s_Data.commandQueue.clear();
    s_Data.activeSkybox = nullptr;
}

void Renderer::BeginScene(const Camera& camera, float aspectRatio)
{
    s_Data.viewMatrix = const_cast<Camera&>(camera).GetViewMatrix();
    s_Data.projectionMatrix = glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.1f, 100.0f);
    s_Data.cameraPosition = camera.Position;

    s_Data.commandQueue.clear();
    s_Data.activeSkybox = nullptr;
}

void Renderer::Submit(Model& model, const glm::mat4& modelMatrix, std::function<void(Shader*)> callback)
{
    float dist = glm::distance(s_Data.cameraPosition, glm::vec3(modelMatrix[3]));
    s_Data.commandQueue.push_back({&model, modelMatrix, callback, dist});
}


void Renderer::SetSkybox(Skybox& skybox)
{
    s_Data.activeSkybox = &skybox;
}

void Renderer::EndScene()
{
    Flush();
}

void Renderer::Flush()
{
    for (const auto& cmd : s_Data.commandQueue)
    {
        if (!cmd.model || !cmd.model->modelShader) continue;
        Shader* shader = cmd.model->modelShader;
        shader->use();
        if (cmd.uniformCallback) cmd.uniformCallback(shader);
        cmd.model->Draw(cmd.modelMatrix, s_Data.viewMatrix, s_Data.projectionMatrix);
    }

    if (s_Data.activeSkybox)
    {
        s_Data.activeSkybox->Draw(s_Data.viewMatrix, s_Data.projectionMatrix);
    }
}
