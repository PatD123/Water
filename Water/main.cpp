#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "common/ShaderHelper.h"
#include "shapes/Cube.h"
#include "camera/Camera.h"

// Paths
const char* VERTEX_SHADER_PATH = "shaders/VertexShader.glsl";
const char* TESSELLATION_CONTROL_SHADER_PATH = "shaders/TessellationControlShader.glsl";
const char* TESSELLATION_EVALUATION_SHADER_PATH = "shaders/TessellationEvaluationShader.glsl";
const char* FRAGMENT_SHADER_PATH = "shaders/FragmentShader.glsl";
const char* COMPUTE_SHADER_PATH = "shaders/ComputeShader.glsl";
const char* DEMO_HEIGHTMAP_PATH = "demo_heightmap.png";

// Timing
float PROGRAM_START_TIME = glfwGetTime();
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Declarated funcs
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

// Making Camera
Camera cam(
    glm::vec3(0.0f, 0.0f, 0.0f),
    FOV,
    ASPECT_RATIO,
    NEAR_PLANE,
    FAR_PLANE
);
bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// Consts
const glm::vec3 LIGHT_POS = glm::vec3(20.0f, 100.0f, 20.0f);
const glm::vec3 LIGHT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);

int main() {

    // Initializing OpenGL windows.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glViewport(0, 0, 800, 600);

    // Set mouse input
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Loading demo heightmap as a texture
    unsigned TEX_WIDTH = 1000;
    unsigned TEX_HEIGHT = 1000;
    unsigned TEX_NCHANNELS = 1; // RGB - Just R - Just height. Tex keeps track of height.

    // Creating r/w heightmap texture for Compute Shader.
    unsigned int heightTexture;
    glGenTextures(1, &heightTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindImageTexture(0, heightTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
    // Creating r/w normal texture for Compute Shader.
    unsigned int normalTexture;
    glGenTextures(1, &normalTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindImageTexture(1, normalTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

    std::cout << "TEXTURE SPECS" << std::endl;
    std::cout << "Height: " << TEX_HEIGHT << std::endl;
    std::cout << "Width: " << TEX_WIDTH << std::endl;
    std::cout << "Number of Channels: " << TEX_NCHANNELS << std::endl;

    // Consts
    const float RES = 40.0f;                     // Number of patches per texture
    const float HPP = TEX_HEIGHT / RES;     // Height per patch
    const float WPP = TEX_WIDTH / RES;      // Width per patch
    const float TEX_HPP = 1.0f / RES;
    const float TEX_WPP = 1.0f / RES;

    std::cout << "Height Per Pixel: " << HPP << std::endl;
    std::cout << "Width Per Pixel: " << WPP << std::endl;
    std::cout << "Tex Height Per Pixel: " << TEX_HPP << std::endl;
    std::cout << "Tex Width Per Pixel: " << TEX_WPP << std::endl;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Shader compilation and linking
    ShaderHelper sh;
    GLuint vShader = sh.compileVShader(VERTEX_SHADER_PATH);
    GLuint tCShader = sh.compileTCShader(TESSELLATION_CONTROL_SHADER_PATH);
    GLuint tEShader = sh.compileTEShader(TESSELLATION_EVALUATION_SHADER_PATH);
    GLuint fShader = sh.compileFShader(FRAGMENT_SHADER_PATH);
    GLuint cShader = sh.compileCShader(COMPUTE_SHADER_PATH);
    GLuint shaderProgram = sh.linkShaders(4, vShader, tCShader, tEShader, fShader);
    GLuint computeShaderProgram = sh.linkShaders(1, cShader);
    glDeleteShader(vShader);
    glDeleteShader(tCShader);
    glDeleteShader(tEShader);
    glDeleteShader(fShader);
    glDeleteShader(cShader);

    // Setting patch parameters
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    /* Patch Generation
    
    We want to divide Height and Width by NUM_PATCHES
    
    height_per_patch = Height / NUM_PATCHES
    width_per_patch = Width / NUM_PATCHES

    i = 0, j = 0
    i * hpp, j * wpp
    */

    std::vector<float> vert_data;

    // Patch generation
    for (size_t i = 0; i < RES; i++) {
        for (size_t j = 0; j < RES; j++) {

            size_t dirs_i[] = { 0, 0, 1, 1 };
            size_t dirs_j[] = { 0, 1, 0, 1 };

            for (size_t k = 0; k < 4; k++) {

                size_t newi = i + dirs_i[k];
                size_t newj = j + dirs_j[k];

                float worldi = newi;
                float worldj = 0.0f;
                float worldk = newj;
                float u = newi * TEX_HPP;
                float v = newj * TEX_WPP;

                vert_data.push_back(worldi);
                vert_data.push_back(worldj);
                vert_data.push_back(worldk);
                vert_data.push_back(u);
                vert_data.push_back(v);

            }

        }
    }

    GLuint VAO, VBO;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    int posLocation = 0;
    int texLocation = 1;

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(posLocation);
    glEnableVertexAttribArray(texLocation);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)* vert_data.size(), vert_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0); // aPos
    glVertexAttribPointer(texLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT))); // aTex
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Make OpenGL do depth testing
    glEnable(GL_DEPTH_TEST);

    // FPS metrics
    double prevTime = 0.0;
    double currTime = 0.0;
    double timeDiff;
    unsigned int frameCounter = 0;

    // Enable V-sync for v-blanks
    glfwSwapInterval(1);

    std::cout << "STARTING TO RENDER" << std::endl;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // we are now around 60 fps, 16-17 ms per frame.
        currTime = glfwGetTime();
        timeDiff = currTime - prevTime;
        frameCounter++;
        if (timeDiff >= 1.0 / 30.0) {
            std::string fps = std::to_string((1.0 / timeDiff) * frameCounter);
            std::string ms = std::to_string((timeDiff / frameCounter) * 1000);
            std::string windowtitle = "fps: " + fps + " | ms: " + ms;
            glfwSetWindowTitle(window, windowtitle.c_str());
            prevTime = currTime;
            frameCounter = 0;
        }

        float currentframe = (float)glfwGetTime();
        deltaTime = currentframe - lastFrame;
        lastFrame = currentframe;

        cam.processMovement(window, deltaTime);
        processInput(window);

        // Clearing screen on each render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        /* Reset color buffer and depth buffer against which OpenGL will compare
        when rendering things on top of each other. */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set shader uniforms
        glm::mat4 world_mat = glm::mat4(1.0f);
        glm::mat4 view_mat = cam.getViewMat();
        sh.setUniformMat4fv(shaderProgram, "world_mat", glm::value_ptr(world_mat));
        sh.setUniformMat4fv(shaderProgram, "view_mat", glm::value_ptr(view_mat));
        sh.setUniformMat4fv(shaderProgram, "proj_mat", glm::value_ptr(Camera::proj_mat));
        sh.setUniform3fv(shaderProgram, "LightPos", glm::value_ptr(LIGHT_POS));
        sh.setUniform3fv(shaderProgram, "LightColor", glm::value_ptr(LIGHT_COLOR));
        sh.setUniform3fv(shaderProgram, "CamPos", glm::value_ptr(cam.m_pos));
        sh.setUniform1f(computeShaderProgram, "t", currentframe);
        sh.setUniform1i(shaderProgram, "heightMap", 0);
        sh.setUniform1i(shaderProgram, "normalMap", 1);

        glUseProgram(computeShaderProgram);
        glDispatchCompute(TEX_WIDTH, TEX_HEIGHT, 1);

        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_PATCHES, 0, 4 * RES * RES);

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

/* For automatically resizing viewport if user changes the viewport size. */
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/* Processes keyboard inputs. */
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/* Changes camera perspective based on mouse location */
void mouseCallback(GLFWwindow* window, double dXPos, double dYPos)
{
    float xpos = static_cast<float>(dXPos);
    float ypos = static_cast<float>(dYPos);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    cam.processMouseMovement(xoffset, yoffset);

    lastX = xpos;
    lastY = ypos;
}


