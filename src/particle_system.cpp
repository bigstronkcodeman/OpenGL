//
// Created by popbox on 12/29/24.
//

#include <particle_system.h>
#include <random>

ParticleSystem::ParticleSystem(Shader* pipelineShaders, ComputeShader* computeShader) : pipelineShaders(pipelineShaders), computeShader(computeShader) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-1, 1);
    std::uniform_real_distribution<float> massDistribution(1, 100.0f);

    for (int i = 0; i < NUM_PARTICLES; i++) {
        glm::vec3 position;
        do {
            position = glm::vec3(
                distribution(generator),
                distribution(generator),
                distribution(generator)
            );
        } while (glm::length(position) > 1.0f);

        Particle p;
        p.position = glm::vec4(position, 1.0f);
        p.velocity = glm::vec4(0.0f);
        p.mass = massDistribution(generator);

        particles.push_back(p);
    }

    glGenBuffers(1, &shaderStorageBufferObject);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderStorageBufferObject);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                  particles.size() * sizeof(Particle),
                  particles.data(),
                  GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, shaderStorageBufferObject);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, shaderStorageBufferObject);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, velocity));
}

void ParticleSystem::update(float deltaTime) {
    computeShader->use();
    computeShader->setFloat("deltaTime", deltaTime);
    glDispatchCompute(static_cast<GLuint>(std::ceil(NUM_PARTICLES / 256.0f)), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void ParticleSystem::render(const glm::mat4& view, const glm::mat4& projection) {
    pipelineShaders->use();
    pipelineShaders->setMat4("view", view);
    pipelineShaders->setMat4("projection", projection);
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, particles.size());
}