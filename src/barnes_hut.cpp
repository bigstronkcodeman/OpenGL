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

BarnesHut::BarnesHut(std::vector<Particle>& particles)
    : particles(particles)
    , octree(glm::vec3(0.0f), 50.01f)
    , forces(particles.size())
{}

void BarnesHut::buildTree() {
    octree.clear(glm::vec3(0.0f), 50.01f);

    for (int i = 0; i < particles.size(); i++) {
        octree.insertParticle(i, particles);
    }
//    octree.updateNodeMasses(0, particles);
    for (int i = octree.getNodes().size() - 1; i >= 0; i--) {
        octree.updateNodeMasses(i, particles);
    }
}

const std::vector<glm::vec3>& BarnesHut::calculateForces(float theta, float softening) {
    const size_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    // Pre-allocate thread-local storage
    std::vector<std::vector<glm::vec3>> threadForces(numThreads);

    auto processParticleRange = [&](size_t threadId, size_t start, size_t end) {
        // Initialize thread-local storage once
        auto& localForces = threadForces[threadId];
        localForces.resize(end - start);

        for (size_t i = start; i < end; i++) {
            glm::vec3 force(0.0f);
            octree.calculateForceRecursive(0, particles[i].position, particles[i].mass,
                                           theta, softening, particles, force);
            localForces[i - start] = force;
        }
    };

    // Divide work among threads
    const size_t particlesPerThread = particles.size() / numThreads;
    for (size_t i = 0; i < numThreads; i++) {
        size_t start = i * particlesPerThread;
        size_t end = (i == numThreads - 1) ? particles.size() : (i + 1) * particlesPerThread;
        threads.emplace_back(processParticleRange, i, start, end);
    }

    // Wait for completion
    for (auto& thread : threads) {
        thread.join();
    }

    // Merge results
    for (size_t t = 0; t < numThreads; t++) {
        size_t start = t * particlesPerThread;
        size_t count = threadForces[t].size();
        std::memcpy(&forces[start], threadForces[t].data(),
                    count * sizeof(glm::vec3));
    }

    return forces;
}

//const std::vector<glm::vec3>& BarnesHut::calculateForces(float theta, float softening) {
//    const size_t minParticlesPerThread = 1000;  // Ensure threads have heavy enough workloads
//    size_t numThreads = std::min((unsigned long long)std::thread::hardware_concurrency(), particles.size() / minParticlesPerThread);
//    if (numThreads == 0) numThreads = 1;  // Fallback to single-threaded if too few particles
//    std::vector<std::thread> threads;
//
//
//    auto processParticleRange = [&](size_t start, size_t end) {
//        std::vector<glm::vec3> localForces(end - start, glm::vec3(0.0f));  // Thread-local storage
//        for (size_t i = start; i < end; i++) {
//            glm::vec3 force(0.0f);
//            octree.calculateForceRecursive(0, particles[i].position, particles[i].mass, theta, softening, particles, force);
//            forces[i] = force;
//        }
//
//        for (size_t i = start; i < end; i++) {
//            forces[i] = localForces[i - start];
//        }
//    };
//
//    const size_t particlesPerThread = particles.size() / numThreads;
//    for (size_t i = 0; i < numThreads; i++) {
//        size_t start = i * particlesPerThread;
//        size_t end = (i == numThreads - 1) ? particles.size() : (i + 1) * particlesPerThread;
//        threads.emplace_back(processParticleRange, start, end);
//    }
//
//    for (auto& thread : threads) {
//        thread.join();
//    }
//
//    return forces;
//}

//ThreadPool pool(std::thread::hardware_concurrency());
//
//const std::vector<glm::vec3>& BarnesHut::calculateForces(float theta, float softening) {
//    const size_t particlesPerThread = particles.size() / std::thread::hardware_concurrency();
//
//    auto processParticleRange = [&](size_t start, size_t end) {
//        for (size_t i = start; i < end; i++) {
//            glm::vec3 force(0.0f);
//            octree.calculateForceRecursive(0, particles[i].position, particles[i].mass, theta, softening, particles, force);
//            forces[i] = force;
//        }
//    };
//
//    for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
//        size_t start = i * particlesPerThread;
//        size_t end = (i == std::thread::hardware_concurrency() - 1) ? particles.size() : (i + 1) * particlesPerThread;
//        pool.enqueue([=]() { processParticleRange(start, end); });
//    }
//
//    return forces;
//}


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

void BarnesHut::renderParticles(const glm::mat4& view, const glm::mat4& projection) {

}

const Octree& BarnesHut::getOctree() {
    return octree;
}