#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "utils/Shader.h"
#include "utils/Camera.h"
#include "utils/Model.h"
#include "utils/Renderer.h"


const std::filesystem::path RESOURCE_ROOT = "/Users/dodge/programs/avr/rta/rta-opengl/src/assignment1";

std::string Path(const std::string& subPath)
{
    return (RESOURCE_ROOT / subPath).string();
}

#define RE(p) Path(p).c_str()


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


const unsigned int window_width = 800;
const unsigned int window_height = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Model aeroplane(RE("aeroplane.glb"), RE("aeroplane.vs"), RE("aeroplane.fs"));

    Renderer::Init();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer::BeginScene(camera, (float)window_width / (float)window_height);
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

            Renderer::Submit(aeroplane, model, [&](Shader* s)
            {
            });
        }
        Renderer::EndScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Renderer::Shutdown();

    glfwTerminate();
    return 0;
}
