//
// Created by popbox on 12/29/24.
//

#include <particle_system.h>
#include <random>

ParticleSystem::ParticleSystem(Shader* pipelineShaders, ComputeShader* computeShader) : pipelineShaders(pipelineShaders), computeShader(computeShader) {
    std::default_random_engine generator(1294);
    std::uniform_real_distribution<float> distribution(-1, 1);
    std::uniform_real_distribution<float> distribution2(-0.3, 0.3);
    std::uniform_real_distribution<float> massDistribution(5000.0f, 200000.0f);

    for (int i = 0; i < NUM_PARTICLES; i++) {
        glm::vec3 position;
        // am lazy, should realistically generate via polar coordinates
        // for starting points inside unit sphere about origin
        do {
            position = glm::vec3(
                distribution(generator),
                distribution(generator),
                distribution2(generator)
            );
        } while (glm::length(position) > 1.0f);

        Particle p;
        p.position = glm::vec4(position, 1.0f);
        glm::vec3 tangent = glm::normalize(glm::cross(glm::vec3(p.position.x, p.position.y, p.position.z), glm::vec3(0.0f, 0.0f, 1.0f))) * 0.05f;
        p.velocity = glm::vec4(tangent.x, tangent.y, tangent.z, 0.0f);
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
    glDispatchCompute(static_cast<GLuint>(std::ceil(NUM_PARTICLES / 128.0f)), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void ParticleSystem::render(const glm::mat4& view, const glm::mat4& projection) {
    pipelineShaders->use();
    pipelineShaders->setMat4("view", view);
    pipelineShaders->setMat4("projection", projection);
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, particles.size());
}

void ParticleSystem::render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    pipelineShaders->use();
    pipelineShaders->setMat4("model", model);
    pipelineShaders->setMat4("view", view);
    pipelineShaders->setMat4("projection", projection);
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, particles.size());
}
