//
// Created by prbou on 1/8/2025.
//

#include <octree.h>

Octree::Octree(const glm::vec3& center, float size) {
    nodes.emplace_back(OctreeNode{
        .centerOfMass = glm::vec3(0.0f),
        .regionCenter = center,
        .size = size,
        .totalMass = 0.0f,
        .firstChildIdx = -1,
        .particleIdx = -1
    });
}

void Octree::insertParticle(int particleIdx, const std::vector<Particle>& particles) {
    insertParticleRecursive(0, particleIdx, particles);
}

void Octree::insertParticleRecursive(int nodeIdx, int particleIdx, const std::vector<Particle>& particles) {
    OctreeNode& node = nodes[nodeIdx];
    const Particle& particle = particles[particleIdx];

    // empty leaf node, just insert the particle
    if (node.firstChildIdx == -1 && node.particleIdx == -1) {
        node.particleIdx = particleIdx;
        updateNodeMasses(nodeIdx, particles);
        return;
    }

    // non-empty leaf node - we need to split it into octants and insert the new
    // particle accordingly, as well as move the existing particle to its new child
    if (node.firstChildIdx == -1) {
        int existingParticleIdx = node.particleIdx;
        const Particle& existingParticle = particles[existingParticleIdx];

        node.firstChildIdx = nodes.size();
        float childSize = 0.5f * node.size;

        for (int i = 0; i < 8; i++) {
            glm::vec3 offset = {
                (i & 4) ? childSize : -childSize,
                (i & 2) ? childSize : -childSize,
                (i & 1) ? childSize : -childSize
            };

            nodes.emplace_back(OctreeNode{
                .centerOfMass = glm::vec3(0.0f),
                .regionCenter = node.regionCenter + offset * 0.5f,
                .size = childSize,
                .totalMass = 0.0f,
                .firstChildIdx = -1,
                .particleIdx = -1
            });
        }

        // node is no longer a leaf, remove its particle
        node.particleIdx = -1;

        // move particle to appropriate child
        int octantForExistingParticle = getOctant(node.regionCenter, existingParticle.position);
        insertParticleRecursive(node.firstChildIdx + octantForExistingParticle, existingParticleIdx, particles);
    }

    // node is not a leaf, insert the new particle into the appropriate child
    int octant = getOctant(node.regionCenter, particle.position);
    insertParticleRecursive(node.firstChildIdx + octant, particleIdx, particles);

    // update node mass to include new particle
    updateNodeMasses(nodeIdx, particles);
}

int Octree::getOctant(const glm::vec3& nodeCenter, const glm::vec3& particlePosition) {
    // encode axis position comparisons into 3-bit number where:
    // - LSB is the x comparison
    // - middle bit is the y comparison
    // - MSB is the z comparison
    // we can use this as our child index offset, since 2^3 - 1 = 7
    return (particlePosition.x >= nodeCenter.x)
        | (particlePosition.y >= nodeCenter.y) << 1
        | (particlePosition.z >= nodeCenter.z) << 2;
}

void Octree::updateNodeMasses(int nodeIdx, const std::vector<Particle>& particles) {
    OctreeNode& node = nodes[nodeIdx];

    if (node.firstChildIdx == -1) {
        // empty leaf node, undefined totalMass
        if (node.particleIdx == -1) {
            node.totalMass = 0.0f;
            return;
        }

        // non-empty leaf node, just use the single particle for the
        // center of totalMass and total totalMass
        const Particle& particle = particles[node.particleIdx];
        node.centerOfMass = particle.position;
        node.totalMass = particle.mass;
        return;
    }

    // non-leaf node, need to update totalMass by accounting for all children
    node.totalMass = 0.0f;
    node.centerOfMass = glm::vec3(0.0f);

    for (int i = 0; i < 8; i++) {
        const OctreeNode& child = nodes[node.firstChildIdx + i];
        if (child.totalMass > 0.0f) {
            node.centerOfMass =
                (node.centerOfMass * node.totalMass + child.centerOfMass * child.totalMass) / (node.totalMass + child.totalMass);
            node.totalMass += child.totalMass;
        }
    }
}

const std::vector<OctreeNode>& Octree::getNodes() {
    return nodes;
}