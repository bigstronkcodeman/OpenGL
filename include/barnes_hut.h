//
// Created by prbou on 1/9/2025.
//

#ifndef OPENGL_FUN_BARNES_HUT_H
#define OPENGL_FUN_BARNES_HUT_H

#include <vector>
#include <glm/glm.hpp>
#include <particle.h>
#include <octree.h>

class BarnesHut {
private:
    std::vector<Particle>& particles;
    std::vector<glm::vec3> forces;
    Octree octree;
public:
    BarnesHut(std::vector<Particle>& particles);
    void buildTree();
    void calculateForces(float theta, float softening);
    void updateParticles(float deltaTime);
    void renderParticles(const glm::mat4& view, const glm::mat4& projection);
    const Octree& getOctree();
};

#endif //OPENGL_FUN_BARNES_HUT_H
