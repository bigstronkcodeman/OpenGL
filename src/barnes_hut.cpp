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
    {
        CpuTimer timer("clear octree");
        octree.clear(glm::vec3(0.0f), OUTER_BOX_SIZE);
    }

    {
        CpuTimer timer("insert particles");
        for (int i = 0; i < particles.size(); i++) {
            octree.insertParticle(i, particles);
        }
    }

    {
        CpuTimer timer("update node masses");
        for (int i = octree.getNodes().size() - 1; i >= 0; i--) {
            octree.updateNodeMasses(i, particles);
        }
    }
}

//void BarnesHut::calculateForces(float theta, float softening) {
//    for (size_t i = 0; i < particles.size(); i++) {
//        glm::vec3 force(0.0f);
//        octree.calculateForce(i, theta, softening, particles, force);
//        forces[i] = force;
//    }
//}

void BarnesHut::calculateForces(float theta, float softening) {
    const size_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    // thread-local storage
    std::vector<std::vector<glm::vec3>> threadForces(numThreads);

    auto processParticleRange = [&](size_t threadId, size_t start, size_t end) {
        // initialize thread-local storage once
        std::vector<glm::vec3>& localForces = threadForces[threadId];
        localForces.resize(end - start);

        for (size_t i = start; i < end; i++) {
            glm::vec3 force(0.0f);
            octree.calculateForceRecursive(0, particles[i].position, particles[i].velocity, particles[i].mass,
                                           theta, softening, particles, force);
            localForces[i - start] = force;
        }
    };

    // divide work among threads
    const size_t particlesPerThread = particles.size() / numThreads;
    for (size_t i = 0; i < numThreads; i++) {
        size_t start = i * particlesPerThread;
        size_t end = (i == numThreads - 1) ? particles.size() : (i + 1) * particlesPerThread;
        threads.emplace_back(processParticleRange, i, start, end);
    }

    // wait for completion
    for (std::thread& thread : threads) {
        thread.join();
    }

    // merge results
    for (size_t t = 0; t < numThreads; t++) {
        size_t start = t * particlesPerThread;
        size_t count = threadForces[t].size();
        std::memcpy(&forces[start], threadForces[t].data(),
                    count * sizeof(glm::vec3));
    }
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