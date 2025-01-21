//
// Created by popbox on 12/29/24.
//

#include <particle_system.h>
#include <random>
#include <iostream>

ParticleSystem::ParticleSystem(DistributionType distributionType, Shader* pipelineShaders, ComputeShader* computeShader)
        : pipelineShaders(pipelineShaders)
        , computeShader(computeShader)
        , currentBuffer(0)
        , particleBuffers()
        , vao() {

    std::default_random_engine generator(12448);

    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle p;
        glm::vec3 position;
        glm::vec3 velocity;

        switch(distributionType) {
            case DistributionType::TANGENTIAL_DONUT: {
                std::uniform_real_distribution<float> distribution(-1, 1);
                std::uniform_real_distribution<float> distribution2(-0.3, 0.3);
//                float meanMass = std::log(5000.0f);
//                float stddev = 2.2f;
//                std::lognormal_distribution<float> mass_dist(meanMass, stddev);
                std::uniform_real_distribution<float> mass_dist(5000.0f, 100000.0f);


                do {
                    position = glm::vec3(
                            distribution(generator),
                            distribution(generator),
                            distribution2(generator)
                    );
                } while (glm::length(position) > 1.0f);

                velocity = glm::normalize(glm::cross(position, glm::vec3(0.0f, 0.0f, 1.0f))) * 0.05f;
                p.mass = mass_dist(generator);
                break;
            }
            case DistributionType::SPIRAL_GALAXY: {
                std::uniform_real_distribution<float> radius_dist(0.0f, 1.0f);
                std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);
                std::uniform_real_distribution<float> z_dist(-0.1f, 0.1f);
//                std::uniform_real_distribution<float> mass_dist(5000.0f, 100000.0f);
                float meanMass = std::log(5000.0f);
                float stddev = 1.5f;
                std::lognormal_distribution<float> mass_dist(meanMass, stddev);

                float radius = radius_dist(generator);
                float angle = angle_dist(generator);
                position = glm::vec3(
                        radius * std::cos(angle),
                        radius * std::sin(angle),
                        z_dist(generator)
                );

                float velocityScale = 0.05f * sqrt(radius);
                velocity = glm::normalize(glm::cross(position, glm::vec3(0.0f, 0.0f, 1.0f))) * velocityScale;
                p.mass = mass_dist(generator);
                break;
            }

            case DistributionType::COLLIDING_WHEELS: {
                std::uniform_real_distribution<float> radius_dist(0.0f, 0.3f);
                std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);
                std::uniform_real_distribution<float> z_dist(-0.1f, 0.1f);
//                std::uniform_real_distribution<float> mass_dist(5000.0f, 100000.0f);
                float meanMass = std::log(5000.0f);
                float stddev = 1.5f;
                std::lognormal_distribution<float> mass_dist(meanMass, stddev);

                bool inFirstGalaxy = i < NUM_PARTICLES/2;
                glm::vec3 center = inFirstGalaxy ?
                                   glm::vec3(-0.5f, 0.0f, 0.0f) :
                                   glm::vec3(0.5f, 0.0f, 0.0f);

                float radius = radius_dist(generator);
                float angle = angle_dist(generator);
                position = center + glm::vec3(
                        radius * std::cos(angle),
                        radius * std::sin(angle),
                        z_dist(generator)
                ) * (inFirstGalaxy ? 1.0f : 2.0f);

                glm::vec3 baseVelocity = inFirstGalaxy ?
                                         glm::vec3(0.02f, 0.0f, 0.0f) :
                                         glm::vec3(-0.02f, 0.0f, 0.0f);
                velocity = baseVelocity + glm::normalize(glm::cross(position - center, glm::vec3(0.0f, 0.0f, 1.0f))) * 0.05f;
                p.mass = mass_dist(generator);
                break;
            }

            case DistributionType::SPHERICAL_SHELL: {
                std::uniform_real_distribution<float> phi_dist(0.0f, 2.0f * M_PI);
                std::uniform_real_distribution<float> costheta_dist(-1.0f, 1.0f);
                float meanMass = std::log(5000.0f);
                float stddev = 1.5f;
                std::lognormal_distribution<float> mass_dist(meanMass, stddev);
//                std::uniform_real_distribution<float> mass_dist(5000.0f, 100000.0f);
                std::uniform_real_distribution<float> vel_dist(0.0f, 0.03f);


                float phi = phi_dist(generator);
                float theta = std::acos(costheta_dist(generator));
                float r = 1.0f;

                position = glm::vec3(
                        r * std::sin(theta) * std::cos(phi),
                        r * std::sin(theta) * std::sin(phi),
                        r * std::cos(theta)
                );

                velocity = -position * 0.01f ;
//                p.mass = mass_dist(generator);
                p.mass = 1000.0f;
                break;
            }

            case DistributionType::BINARY_SYSTEM: {
                std::normal_distribution<float> position_dist(0.0f, 0.3f);
//                std::uniform_real_distribution<float> mass_dist(5000.0f, 100000.0f);
                float meanMass = std::log(5000.0f);
                float stddev = 1.5f;
                std::lognormal_distribution<float> mass_dist(meanMass, stddev);

                bool inFirstCluster = i < NUM_PARTICLES/2;
                glm::vec3 center = inFirstCluster ?
                                   glm::vec3(-0.5f, 0.0f, 0.0f) :
                                   glm::vec3(0.5f, 0.0f, 0.0f);

                position = center + glm::vec3(
                        position_dist(generator),
                        position_dist(generator),
                        position_dist(generator)
                );

                velocity = glm::normalize(glm::cross(position, glm::vec3(0.0f, 1.0f, 0.0f))) * 0.05f;
                p.mass = mass_dist(generator);
                break;
            }
            case DistributionType::UNIT_SPHERE: {
                float meanMass = std::log(5000.0f);
                float stddev = 1.5f;
                std::lognormal_distribution<float> mass_dist(meanMass, stddev);
                std::uniform_real_distribution<float> pos_dist(-1.0f, 1.0f);

                p.mass = mass_dist(generator);
                do {
                    position = glm::vec4(
                        pos_dist(generator),
                        pos_dist(generator),
                        pos_dist(generator),
                        0
                    );
                } while (glm::length(position) >= 1);
                velocity = glm::vec4(0.0f);
                break;
            }
        }

        p.position = glm::vec4(position, 1.0f);
        p.velocity = glm::vec4(velocity, 0.0f);

        for (auto & previousPosition : p.previousPositions) {
            previousPosition = p.position;
        }

        particles.push_back(p);
    }

    for (int i = 0; i < particleBuffers.size(); i++) {
        particleBuffers[i].allocate(particles.size() * sizeof(Particle),
                                    particles.data(),
                                    GL_DYNAMIC_DRAW);
        particleBuffers[i].bindToPoint(i);
    }

    VertexAttribute vertexAttribute{
        .index = 0,
        .size = 4,
        .type = GL_FLOAT,
        .normalized = GL_FALSE,
        .stride = sizeof(Particle),
        .offset = (void*)offsetof(Particle, position)
    };

    vao.bind();
    particleBuffers[currentBuffer].bind();
    vao.setVertexAttributePointers({vertexAttribute});
}


void ParticleSystem::update(float deltaTime) {
    computeShader->use();
    computeShader->setFloat("deltaTime", deltaTime);

    particleBuffers[currentBuffer].bindToPoint(0);
    particleBuffers[1 - currentBuffer].bindToPoint(1);

    glDispatchCompute(static_cast<GLuint>(std::ceil(NUM_PARTICLES / 1024.0f)), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    currentBuffer = 1 - currentBuffer;
}

void ParticleSystem::render(const glm::mat4& view, const glm::mat4& projection) {
    pipelineShaders->use();
    pipelineShaders->setMat4("view", view);
    pipelineShaders->setMat4("projection", projection);
    pipelineShaders->setInt("currentBuffer", currentBuffer);

    vao.bind();
    particleBuffers[currentBuffer].bind();
    glDrawArrays(GL_POINTS, 0, particles.size() * 17);
}

void ParticleSystem::render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    pipelineShaders->use();
    pipelineShaders->setMat4("model", model);
    pipelineShaders->setMat4("view", view);
    pipelineShaders->setMat4("projection", projection);

    vao.bind();
    particleBuffers[currentBuffer].bind();
    glDrawArrays(GL_POINTS, 0, particles.size() * 17);
}

std::vector<Particle>& ParticleSystem::getParticles() {
    return particles;
}