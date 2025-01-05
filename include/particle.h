//
// Created by popbox on 12/27/24.
//

#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

struct Particle {
    glm::vec4 position;
    glm::vec4 velocity;
    float mass;
    float padding[3];
};

#endif //PARTICLE_H
