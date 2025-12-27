#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "common/ShaderHelper.h"

const char* VERTEX_SHADER_PATH = "shaders/VertexShader.glsl";
const char* FRAGMENT_SHADER_PATH = "shaders/FragmentShader.glsl";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main() {

    // Initializing OpenGL windows.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Making the window
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Loading GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setting viewport size.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, 800, 600);

    // Shader compilation and linking
    ShaderHelper sh;
    GLuint vShader = sh.compileVShader(VERTEX_SHADER_PATH);
    GLuint fShader = sh.compileFShader(FRAGMENT_SHADER_PATH);
    GLuint shaderProgram = sh.linkShaders(2, vShader, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Processing keyboard inputs
        processInput(window);

        // Clearing screen on each render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /*
        Typically applications have two buffers. One is the front one that holds what is currently
        being shown on the screen. The back buffer is the one we are updating. When the update is done, 
        we simply swap the two.
        */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleaning up the GLFW window.
    glfwTerminate();

    return 0;
}

/*
For automatically resizing viewport if user changes the viewport size.
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/*
Processes keyboard inputs.
*/
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

