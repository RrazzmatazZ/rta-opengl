#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <iostream>

#include "utils/Shader.h"
#include "utils/Camera.h"
#include "utils/Model.h"
#include "utils/Renderer.h"
#include "utils/Skybox.h"

#pragma region window and camera
const unsigned int window_width = 1920;
const unsigned int window_height = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
#pragma endregion window and camera

int rotationMode = 0;
glm::vec3 eulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
glm::quat currentQuat = glm::identity<glm::quat>();

float deltaTime = 0.0f;
float lastFrame = 0.0f;


#pragma region file path RE
const std::filesystem::path RESOURCE_ROOT = "/Users/dodge/programs/avr/rta/rta-opengl/src/assignment1";

std::string Path(const std::string& subPath)
{
    return (RESOURCE_ROOT / subPath).string();
}

#define RE(p) Path(p).c_str()
#pragma endregion


#pragma region path animation

glm::vec3 bezierPoint(float t, glm::vec3 start, glm::vec3 p1, glm::vec3 p2, glm::vec3 end)
{
    const float el = 1 - t;
    //B(t) = (1-t)^3 P_0 + 3(1-t)^2 t P_1 + 3(1-t) t^2 P_2 + t^3 P_3
    return el * el * el * start + 3 * el * el * t * p1 + 3 * el * t * t * p2 + t * t * t * end;
}

glm::vec3 bezierPointLookAt(float t, glm::vec3 start, glm::vec3 p1, glm::vec3 p2, glm::vec3 end)
{
    const float el = 1 - t;
    //B'(t) = 3*(1-t)^2*(p1-p0) + 6*(1-t)*t*(p2-p1) + 3*t^2*(p3-p2)
    return 3 * el * el * (p1 - start) + 6 * el * t * (p2 - p1) + 3 * t * t * (end - p2);
}

Mesh GenerateCubic(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (int i = 0; i <= segments; ++i)
    {
        float t = (float)i / (float)segments;
        glm::vec3 pos = bezierPoint(t, p0, p1, p2, p3);

        Vertex v;
        v.Position = pos;
        vertices.push_back(v);
        indices.push_back(i);
    }

    return Mesh(vertices, indices, textures, GL_LINE_STRIP);
}

#pragma endregion path animation

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Aeroplane Simulate", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

#pragma region imgui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
#pragma endregion imgui


    Model aeroplane(RE("aeroplane.glb"), RE("aeroplane.vs"), RE("aeroplane.fs"));

    std::vector<std::string> skybox_paths = {
        RE("skybox/miramar_lf.tga"),
        RE("skybox/miramar_rt.tga"),
        RE("skybox/miramar_up.tga"),
        RE("skybox/miramar_dn.tga"),
        RE("skybox/miramar_ft.tga"),
        RE("skybox/miramar_bk.tga"),

    };

    Skybox skybox = Skybox(skybox_paths,RE("skybox/skybox.vs"), RE("skybox/skybox.fs"));


    Shader* lineShader = new Shader("line.vs", "line.fs");

    float len = 80.0f;
    float width = 20.0f;
    float mid = len / 2.0f;
    int segments = 800;

    std::vector<Mesh> meshes;

    meshes.push_back(GenerateCubic(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, width),
        glm::vec3(mid - 3.0f, 0.0f, width),
        glm::vec3(mid, 0.0f, 0.0f),
        segments
    ));

    meshes.push_back(GenerateCubic(
        glm::vec3(mid, 0.0f, 0.0f),
        glm::vec3(mid + 3.0f, 0.0f, -width),
        glm::vec3(len, 0.0f, -width),
        glm::vec3(len, 0.0f, 0.0f),
        segments
    ));

    meshes.push_back(GenerateCubic(
        glm::vec3(len, 0.0f, 0.0f),
        glm::vec3(len, 0.0f, width),
        glm::vec3(mid + 3.0f, 0.0f, width),
        glm::vec3(mid, 0.0f, 0.0f),
        segments
    ));

    meshes.push_back(GenerateCubic(
        glm::vec3(mid, 0.0f, 0.0f),
        glm::vec3(mid - 3.0f, 0.0f, -width),
        glm::vec3(0.0f, 0.0f, -width),
        glm::vec3(0.0f, 0.0f, 0.0f),
        segments
    ));

    Model* bezierCurveModel = new Model(meshes, lineShader);

    Renderer::Init();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region imgui panel
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(380, 280), ImGuiCond_Always);
            ImGui::Begin("Aeroplane Control Panels");

            ImGui::Text("Rotation Mode:");
            ImGui::RadioButton("Euler", &rotationMode, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Quaternion", &rotationMode, 1);

            if (rotationMode == 0)
            {
                ImGui::Text("Euler Panel");
                ImGui::SliderFloat("Pitch (X)", &eulerAngles.x, -90.0f, 90.0f);
                ImGui::SliderFloat("Yaw   (Y)", &eulerAngles.y, -180.0f, 180.0f);
                ImGui::SliderFloat("Roll  (Z)", &eulerAngles.z, -180.0f, 180.0f);
            }

            if (rotationMode == 1)
            {
                ImGui::Text("Quaternion Panel");
                float rotSpeed = 500.0f * deltaTime;

                glm::quat qPitch = glm::angleAxis(glm::radians(rotSpeed), glm::vec3(1, 0, 0));
                glm::quat qYaw = glm::angleAxis(glm::radians(rotSpeed), glm::vec3(0, 1, 0));
                glm::quat qRoll = glm::angleAxis(glm::radians(rotSpeed), glm::vec3(0, 0, 1));

                ImGui::Text("Pitch (X):");
                ImGui::SameLine();
                if (ImGui::Button("Up##P")) currentQuat = currentQuat * qPitch;
                ImGui::SameLine();
                if (ImGui::Button("Down##P")) currentQuat = currentQuat * glm::inverse(qPitch);
                ImGui::Text("Yaw   (Y):");
                ImGui::SameLine();
                if (ImGui::Button("Left##Y")) currentQuat = currentQuat * qYaw;
                ImGui::SameLine();
                if (ImGui::Button("Right##Y")) currentQuat = currentQuat * glm::inverse(qYaw);
                ImGui::Text("Roll  (Z):");
                ImGui::SameLine();
                if (ImGui::Button("CCW##R")) currentQuat = currentQuat * qRoll;
                ImGui::SameLine();
                if (ImGui::Button("CW##R")) currentQuat = currentQuat * glm::inverse(qRoll);
            }
            if (ImGui::Button("Reset Orientation"))
            {
                eulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
                currentQuat = glm::identity<glm::quat>();
            }
            ImGui::NewLine();

            ImGui::Text("Fly Simulation Panel");
            if (ImGui::Button("Start"))
            {
                camera.Position = glm::vec3(40.0f, 60.0f, 0.0f);
                camera.Pitch = -89.9f;
                camera.Yaw = -90.0f;
                camera.updateCameraVectors();
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop"))
            {
                camera.Position = glm::vec3(0.0f, 0.0f, 10.0f);
                camera.Pitch = 0.0f;
                camera.Yaw = -90.0f;
                camera.updateCameraVectors();
            }
            ImGui::End();
        }
#pragma endregion

        Renderer::BeginScene(camera, (float)window_width / (float)window_height);
        Renderer::SetSkybox(skybox);
        Renderer::Submit(*bezierCurveModel, glm::mat4(1.0f));
        {
            glm::mat4 eulerMatrix = glm::mat4(1.0f);

            //yxz order euler
            eulerMatrix = glm::rotate(eulerMatrix, glm::radians(eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
            eulerMatrix = glm::rotate(eulerMatrix, glm::radians(eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
            eulerMatrix = glm::rotate(eulerMatrix, glm::radians(eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 quatMatrix = glm::mat4_cast(glm::normalize(currentQuat));

            glm::mat4 baseMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            baseMatrix = glm::rotate(baseMatrix, glm::radians(-90.0f), glm::vec3(.0f, 1.0f, 0.0f));
            baseMatrix = glm::rotate(baseMatrix, glm::radians(-90.0f), glm::vec3(1.0f, .0f, 0.0f));
            baseMatrix = glm::scale(baseMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

            baseMatrix = eulerMatrix * quatMatrix * baseMatrix;

            Renderer::Submit(aeroplane, baseMatrix);
        }
        Renderer::EndScene();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Renderer::Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
