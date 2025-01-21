//
// Created by prbou on 1/9/2025.
//

#include <thread>
#include <glm/glm.hpp>
#include <barnes_hut.h>
#include <iostream>
#include <future>
#include <cstring>
#include "thread_pool.h"
#include "timers.h"

constexpr float OUTER_BOX_SIZE = 5.0f;

BarnesHut::BarnesHut(std::vector<Particle>& particles)
    : particles(particles)
    , octree(glm::vec3(0.0f), OUTER_BOX_SIZE)
    , forces(particles.size())
{}

void BarnesHut::buildTree() {
    octree.clear(glm::vec3(0.0f), OUTER_BOX_SIZE);

    for (int i = 0; i < particles.size(); i++) {
        octree.insertParticle(i, particles);
    }

    for (int i = octree.getNodes().size() - 1; i >= 0; i--) {
        octree.updateNodeMasses(i, particles);
    }
}

//void BarnesHut::calculateForces(float theta, float softening) {
//    for (size_t i = 0; i < particles.size(); i++) {
//        glm::vec3 force(0.0f);
//        octree.calculateForceIterative(0, particles[i].position, particles[i].velocity, particles[i].mass, theta, softening, particles, force);
//        forces[i] = force;
//    }
//}

ThreadPool pool(std::thread::hardware_concurrency());

void BarnesHut::calculateForces(float theta, float softening) {
    const size_t numThreads = std::thread::hardware_concurrency();
    const size_t particlesPerThread = particles.size() / numThreads;

    auto processParticleRange = [&](size_t start, size_t end) {
        for (size_t i = start; i < end; i++) {
            glm::vec3 force(0.0f);
            octree.calculateForceIterative(0, particles[i].position, particles[i].velocity,
                                           particles[i].mass, theta, softening, particles, force);
            forces[i] = force;
        }
    };

    for (size_t i = 0; i < numThreads; i++) {
        size_t start = i * particlesPerThread;
        size_t end = (i == numThreads - 1) ? particles.size() : (i + 1) * particlesPerThread;
        pool.enqueue([=]() { processParticleRange(start, end); });
    }

    pool.wait_completion();
}


void BarnesHut::updateParticles(float deltaTime) {
    for (size_t i = 0; i < particles.size(); i++) {
        glm::vec3 acceleration = forces[i] / particles[i].mass;
        Particle& particle = particles[i];
        particle.velocity += glm::vec4(acceleration * deltaTime, 1.0f);

//        for (size_t j = POSITION_HISTORY_SIZE - 1; j > 0; j--) {
//            particle.previousPositions[j] = particle.previousPositions[j-1];
//        }
//        particle.previousPositions[0] = particle.position;

        particle.position += glm::vec4(
            particle.velocity.x * deltaTime,
            particle.velocity.y * deltaTime,
            particle.velocity.z * deltaTime,
            0.0f);
    }
}

void BarnesHut::renderParticles(const glm::mat4& view, const glm::mat4& projection) {

}

const Octree& BarnesHut::getOctree() {
    return octree;
}