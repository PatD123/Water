#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/ShaderHelper.h"
#include "shapes/Cube.h"
#include "camera/Camera.h"

const char* VERTEX_SHADER_PATH = "shaders/VertexShader.glsl";
const char* FRAGMENT_SHADER_PATH = "shaders/FragmentShader.glsl";

// Timing
float PROGRAM_START_TIME = glfwGetTime();
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

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
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glViewport(0, 0, 800, 600);

    // Set mouse input
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Shader compilation and linking
    ShaderHelper sh;
    GLuint vShader = sh.compileVShader(VERTEX_SHADER_PATH);
    GLuint fShader = sh.compileFShader(FRAGMENT_SHADER_PATH);
    GLuint shaderProgram = sh.linkShaders(2, vShader, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    GLuint VAO, VBO;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    int posLocation = 0;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(posLocation);
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Cube::vertices), Cube::vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Make OpenGL do depth testing
    glEnable(GL_DEPTH_TEST);

    // FPS metrics
    double prevTime = 0.0;
    double currTime = 0.0;
    double timeDiff;
    unsigned int frameCounter = 0;

    // Enable V-sync for v-blanks
    glfwSwapInterval(1);

    std::cout << "Starting to render" << std::endl;

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

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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


