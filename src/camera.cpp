//
// Created by popbox on 12/25/24.
//

#include "camera.h"
#include <ostream>

FirstPersonCamera::FirstPersonCamera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp,
                                     float yaw, float pitch, float speed, float mouseSensitivity, float fov)
                                         : position(position), forward(direction), worldUp(worldUp)
                                         , yaw(yaw), pitch(pitch), speed(speed), mouseSensitivity(mouseSensitivity), fov(fov)
{ updateVectors(); }

void FirstPersonCamera::processMouseMovement(double xdiff, double ydiff) {
    xdiff *= mouseSensitivity;
    ydiff *= mouseSensitivity;

    yaw -= xdiff;
    pitch = std::min(89.0f, std::max(-89.0f, pitch - (float)ydiff));

    updateVectors();
}

void FirstPersonCamera::processMouseScroll(double ydiff) {
    fov = std::max(1.0f, std::min(45.0f, fov - (float)ydiff));
}


void FirstPersonCamera::processKeyboardInput(CameraDirection direction, float deltaTime) {
    float movementScalar = speed * deltaTime;
    switch(direction) {
        case UP:
            position += movementScalar * up;
            break;
        case DOWN:
            position -= movementScalar * up;
            break;
        case RIGHT:
            position -= movementScalar * glm::normalize(glm::cross(up, forward));
            break;
        case LEFT:
            position += movementScalar * glm::normalize(glm::cross(up, forward));
            break;
        case FORWARD:
            position += movementScalar * forward;
            break;
        case BACKWARD:
            position -= movementScalar * forward;
            break;
    }
}

glm::mat4 FirstPersonCamera::getViewTransform() const {
    return {
        right.x, up.x, -forward.x, 0,
        right.y, up.y, -forward.y, 0,
        right.z, up.z, -forward.z, 0,
        -position.x*right.x - position.y*right.y - position.z*right.z,
            -position.x*up.x - position.y*up.y - position.z*up.z,
            position.x*forward.x + position.y*forward.y + position.z*forward.z,
            1
 };
}

void FirstPersonCamera::updateVectors() {
    forward.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    forward.y = std::sin(glm::radians(pitch));
    forward.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    forward = glm::normalize(forward);

    right = -glm::normalize(glm::cross(forward, worldUp));
    up = glm::cross(right, forward);
}
