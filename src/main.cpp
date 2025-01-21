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
#include <timers.h>

#include "particle_system.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
constexpr bool BARNES_HUT = true;

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
bool drawOctree = false;
bool pause = false;

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
    Shader naiveParticleShaders("../shaders/naiveParticlesVert.glsl", "../shaders/naiveParticlesFrag.glsl");
    Shader barnesHutParticleShaders("../shaders/barnesHutParticlesVert.glsl", "../shaders/barnesHutParticlesFrag.glsl");
    Shader octreeShaders("../shaders/octreeVert.glsl", "../shaders/octreeFrag.glsl");
    ComputeShader forceCalculationShader("../shaders/calculateForces.glsl");
    ParticleSystem particleSystem(DistributionType::COLLIDING_WHEELS, BARNES_HUT ? &barnesHutParticleShaders : &naiveParticleShaders, &forceCalculationShader);
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

    naiveParticleShaders.use();

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

        // rotate camera continually around origin
        glm::vec3 cameraPos(3.0f * std::cos(camera.yaw), 0.0f, 3.0f * std::sin(camera.yaw));
        camera.position = cameraPos;
        camera.forward = -glm::normalize(cameraPos);
        camera.right = glm::normalize(glm::cross(camera.forward, camera.worldUp));
        camera.up = glm::cross(camera.right, camera.forward);
        camera.yaw += .01f;

        // create transformations
        glm::mat4 view = camera.getViewTransform();
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.0001f, 100.0f);

        if (BARNES_HUT) {
            {
                CpuTimer timer("Build tree");
                barnesHut.buildTree();
            }
            if (!pause) {
                CpuTimer timer("Calculate forces");
                barnesHut.calculateForces(0.5f, 0.02);
            }
            if (!pause) {
                CpuTimer timer("Update particles");
                barnesHut.updateParticles(deltaTime);
            }

            if (drawOctree) {
                octreeShaders.use();
                octreeShaders.setMat4("view", view);
                octreeShaders.setMat4("projection", projection);
                {
                    Timer timer("Draw bounding boxes");
                    barnesHut.getOctree().drawBoundingBoxes();
                }
            }

            barnesHutParticleShaders.use();
            barnesHutParticleShaders.setMat4("view", view);
            barnesHutParticleShaders.setMat4("projection", projection);
            {
                Timer timer("Move particles to GPU and draw");
                vao.allocate(particleSystem.getParticles().size() * sizeof(Particle),
                             particleSystem.getParticles().data(),
                             GL_DYNAMIC_DRAW);
                glDrawArrays(GL_POINTS, 0, particleSystem.getParticles().size() /** (1 + POSITION_HISTORY_SIZE)*/);
            }
        } else {
            if (!pause) {
                particleSystem.update(deltaTime);
            }
            particleSystem.render(view, projection);
        }

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
    static constexpr double KEY_PRESS_WAIT_TIME = 0.1;
    static double lastKeyPressedTime = 0.0f;
    double currentTime = glfwGetTime();

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
        camera.speed = std::max(0.01f, camera.speed - 0.01f);

    // commands that should wait before getting re-triggered
    if (currentTime - lastKeyPressedTime > KEY_PRESS_WAIT_TIME) {
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            drawOctree = !drawOctree;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            pause = !pause;
        }
        lastKeyPressedTime = currentTime;
    }

}

// whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions;
    glViewport(0, 0, width, height);
}
