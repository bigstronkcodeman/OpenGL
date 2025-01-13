//
// Created by prbou on 1/9/2025.
//

#include <thread>
#include <glm/glm.hpp>
#include <barnes_hut.h>

BarnesHut::BarnesHut(std::vector<Particle>& particles)
    : particles(particles)
    , octree(Octree(glm::vec3(0.0f), 1.01f))
    , forces(particles.size())
{}

const std::vector<glm::vec3>& BarnesHut::calculateForces(float theta, float softening) {
    static const size_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    auto processParticleRange = [&](size_t start, size_t end) {
        for (size_t i = start; i < end; i++) {
            glm::vec3 force(0.0f);
            octree.calculateForceRecursive(0, particles[i].position, particles[i].mass, theta, softening, particles, force);
            forces[i] = force;
        }
    };

    const size_t particlesPerThread = particles.size() / numThreads;
    for (size_t i = 0; i < numThreads; i++) {
        size_t start = i * particlesPerThread;
        size_t end = (i == numThreads - 1) ? particles.size() : (i + 1) * particlesPerThread;
        threads.emplace_back(processParticleRange, start, end);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return forces;
}

void BarnesHut::updateParticles(float deltaTime) {
    for (size_t i = 0; i < particles.size(); i++) {
        glm::vec3 acceleration = forces[i] / particles[i].mass;
        Particle& particle = particles[i];
        particle.velocity += glm::vec4(acceleration * deltaTime, 1.0f);

        for (size_t j = POSITION_HISTORY_SIZE - 1; j > 0; j--) {
            particle.previousPositions[j] = particle.previousPositions[j-1];
        }
        particle.previousPositions[0] = particle.position;

        particle.position += glm::vec4(
            particle.velocity.x * deltaTime,
            particle.velocity.y * deltaTime,
            particle.velocity.z * deltaTime,
            0.0f);
    }
}

