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
#include "utils/Skybox.h"

const unsigned int window_width = 1920;
const unsigned int window_height = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = (float)window_width / 2.0;
float lastY = (float)window_height / 2.0;
bool firstMouse = true;


float deltaTime = 0.0f;
float lastFrame = 0.0f;

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

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

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

    Renderer::Init();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer::BeginScene(camera, (float)window_width / (float)window_height);
        Renderer::SetSkybox(skybox);
        {



            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, .0f, 0.0f));
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
