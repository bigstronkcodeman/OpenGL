//
// Created by prbou on 1/8/2025.
//

#ifndef OPENGL_FUN_OCTREE_H
#define OPENGL_FUN_OCTREE_H

#include <vector>
#include <octree_node.h>
#include <particle.h>

class Octree {
private:
    std::vector<OctreeNode> nodes;
public:
    Octree(const glm::vec3& center, float size);
    void insertParticle(int particleIdx, const std::vector<Particle>& particles);
    void insertParticleRecursive(int nodeIdx, int particleIdx, const std::vector<Particle>& particles);
    static int getOctant(const glm::vec3& nodeCenter, const glm::vec3& particlePosition);
    void updateNodeMasses(int nodeIdx, const std::vector<Particle>& particles);
    const std::vector<OctreeNode>& getNodes();
    void calculateForce(int particleIdx, float theta, float softening, const std::vector<Particle>& particles, glm::vec3& force);
    void calculateForceRecursive(int nodeIdx, const glm::vec3& position, float mass, float theta, float softening, const std::vector<Particle>& particles, glm::vec3& force);
};

#endif //OPENGL_FUN_OCTREE_H
