//
// Created by popbox on 12/27/24.
//

#ifndef PARTICLE_H
#define PARTICLE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

constexpr GLuint POSITION_HISTORY_SIZE = 16;

struct Particle {
    glm::vec4 position;
    glm::vec4 velocity;
    float mass;
    float padding[3];
    glm::vec4 previousPositions[POSITION_HISTORY_SIZE];
};

#endif //PARTICLE_H
