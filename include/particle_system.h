//
// Created by popbox on 12/29/24.
//

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <array>
#include <glad/glad.h>
#include <particle.h>
#include <shader.h>
#include <compute_shader.h>
#include <buffer_manager.h>


enum class DistributionType {
    TANGENTIAL_DONUT,
    SPIRAL_GALAXY,
    COLLIDING_WHEELS,
    SPHERICAL_SHELL,
    BINARY_SYSTEM,
    UNIT_SPHERE
};


class ParticleSystem {
    Shader* pipelineShaders;
    ComputeShader* computeShader;
    std::vector<Particle> particles;
    std::array<ShaderStorageBuffer, 2> particleBuffers;
    GLuint currentBuffer;
    VertexArrayObject vao;
    const int NUM_PARTICLES = 10000;

public:
    ParticleSystem(DistributionType distributionType, Shader* pipelineShaders, ComputeShader* computeShader);
    void update(float deltaTime);
    void render(const glm::mat4& view, const glm::mat4& projection);
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    std::vector<Particle>& getParticles();
};

#endif //PARTICLESYSTEM_H
