#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <compute_shader.h>
#include <camera.h>
#include <barnes_hut.h>

#include "particle_system.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
FirstPersonCamera camera(glm::vec3(0.0f, 0.0f, 3.0f),
                         glm::vec3(0.0f, 0.0f, -1.0f),
                         glm::vec3(0.0f, 1.0f, 0.0f));


float pitch = 0.0f;
float yaw = -90.0f;

// misc state
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastMouseX = 400.0f;
float lastMouseY = 300.0f;
bool firstMouse = true;
float mixin = 0.0f;

// process mouse movement
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    camera.processMouseMovement(xpos - lastMouseX, lastMouseY - ypos);
    lastMouseX = xpos;
    lastMouseY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(yoffset);
}

GLFWwindow* initializeGlfwWindow() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gravity Sim", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
//     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        std::exit(-1);
    }

    return window;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
    return os << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
}

int main() {
    GLFWwindow *window = initializeGlfwWindow();

    // build and compile shader program
    Shader pipelineShaders("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    Shader defaultShaders("../shaders/defaultVertexShader.glsl", "../shaders/defaultFragmentShader.glsl");
//    ComputeShader octreeBuilderShader("../shaders/buildTree.glsl");
    ComputeShader forceCalculationShader("../shaders/calculateForces.glsl");
    ParticleSystem particleSystem(DistributionType::SPHERICAL_SHELL, &pipelineShaders, &forceCalculationShader);
    BarnesHut barnesHut(particleSystem.getParticles());

    VertexArrayObject vao;
    VertexAttribute vertexAttribute{
        .index = 0,
        .size = 4,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Particle),
        .offset = (void*)offsetof(Particle, position)
    };
    vao.setVertexAttributePointers({vertexAttribute});

    // activate depth buffer culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    pipelineShaders.use();
    camera.yaw = 0;

    // render loop
    float currentFrame;
    while (!glfwWindowShouldClose(window)) {
        currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        glm::vec3 cameraPos(5.0f * std::cos(camera.yaw), 0.0f, 5.0f * std::sin(camera.yaw));
//        camera.position = cameraPos;
//        camera.forward = -glm::normalize(cameraPos);
//        camera.right = glm::normalize(glm::cross(camera.forward, camera.worldUp));
//        camera.up = glm::cross(camera.right, camera.forward);
//        camera.yaw += .004f;

        // create transformations
//        glm::mat4 view = camera.getViewTransform();
        glm::mat4 view = glm::lookAt(camera.position, camera.position + camera.forward, camera.worldUp);
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // std::cout << "camera forward: " << camera.forward << '\n';
        // std::cout << "camera position: " << camera.position << '\n';

        barnesHut.buildTree();
        barnesHut.calculateForces(0.5f, 0.5);
        barnesHut.updateParticles(deltaTime);

        defaultShaders.use();
        defaultShaders.setMat4("view", view);
        defaultShaders.setMat4("projection", projection);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe mode
        barnesHut.getOctree().drawBoundingBoxes();

        pipelineShaders.use();
        pipelineShaders.setMat4("view", view);
        pipelineShaders.setMat4("projection", projection);
        vao.allocate(particleSystem.getParticles().size() * sizeof(Particle),
                     particleSystem.getParticles().data(),
                     GL_DYNAMIC_DRAW);
        glDrawArrays(GL_POINTS, 0, particleSystem.getParticles().size() * (1 + POSITION_HISTORY_SIZE));
//        particleSystem.update(deltaTime);
//        particleSystem.render(view, projection);

        glFinish();

        // swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // terminate, clearing all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        mixin = std::min(1.0f, mixin + 0.01f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        mixin = std::max(0.0f, mixin - 0.01f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.speed += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.speed -= 0.01f;

}

// whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions;
    glViewport(0, 0, width, height);
}
