//
// Created by popbox on 12/29/24.
//

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <glad/glad.h>
#include <particle.h>
#include <shader.h>
#include <compute_shader.h>

class ParticleSystem {
    Shader* pipelineShaders;
    ComputeShader* computeShader;
    GLuint vao;
    GLuint shaderStorageBufferObject;
    std::vector<Particle> particles;
    const int NUM_PARTICLES = 50000;

public:
    ParticleSystem(Shader* pipelineShaders, ComputeShader* computeShader);
    void update(float deltaTime);
    void render(const glm::mat4& view, const glm::mat4& projection);
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
};

#endif //PARTICLESYSTEM_H
