//
// Created by popbox on 12/25/24.
//

#include <glm/glm.hpp>

#ifndef CAMERA_H
#define CAMERA_H

enum CameraDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

namespace CameraDefaults {
    const glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 3.0f);
    const glm::vec3 DIRECTION = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 RIGHT = glm::normalize(glm::cross(UP, DIRECTION));
    const float INITIAL_YAW = -90.0f;
    const float INITIAL_PITCH = 0.0f;
    const float DEFAULT_SPEED = 2.5f;
    const float DEFAULT_MOUSE_SENSITIVITY = 0.05f;
    const float INITIAL_FOV = 45.0f;
};

class FirstPersonCamera {
public:
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up{};
    glm::vec3 right{};
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float speed;
    float mouseSensitivity;
    float fov;

    FirstPersonCamera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp,
                      float yaw = CameraDefaults::INITIAL_YAW, float pitch = CameraDefaults::INITIAL_PITCH,
                      float speed = CameraDefaults::DEFAULT_SPEED, float mouseSensitivity = CameraDefaults::DEFAULT_MOUSE_SENSITIVITY,
                      float fov = CameraDefaults::INITIAL_FOV);
    void processMouseMovement(double xdiff, double ydiff);
    void processMouseScroll(double ydiff);
    void processKeyboardInput(CameraDirection direction, float deltaTime);
    glm::mat4 getViewTransform() const;

// private:
    void updateVectors();
};

#endif //CAMERA_H
