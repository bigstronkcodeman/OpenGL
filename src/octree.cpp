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
//    OctreeNode& node = nodes[nodeIdx];
    const Particle& particle = particles[particleIdx];

    // empty leaf node, just insert the particle
    if (nodes[nodeIdx].firstChildIdx == -1 && nodes[nodeIdx].particleIdx == -1) {
        nodes[nodeIdx].particleIdx = particleIdx;
        updateNodeMasses(nodeIdx, particles);
        return;
    }

    // non-empty leaf node - we need to split it into octants and insert the new
    // particle accordingly, as well as move the existing particle to its new child
    if (nodes[nodeIdx].firstChildIdx == -1) {
        int existingParticleIdx = nodes[nodeIdx].particleIdx;
        const Particle& existingParticle = particles[existingParticleIdx];

        nodes[nodeIdx].firstChildIdx = nodes.size();
        float childSize = 0.5f * nodes[nodeIdx].size;

        for (int i = 0; i < 8; i++) {
            glm::vec3 offset = {
                (i & 4) ? childSize : -childSize,
                (i & 2) ? childSize : -childSize,
                (i & 1) ? childSize : -childSize
            };

            nodes.emplace_back(OctreeNode{
                .centerOfMass = glm::vec3(0.0f),
                .regionCenter = nodes[nodeIdx].regionCenter + offset * 0.5f,
                .size = childSize,
                .totalMass = 0.0f,
                .firstChildIdx = -1,
                .particleIdx = -1
            });
        }

        // node is no longer a leaf, remove its particle
        nodes[nodeIdx].particleIdx = -1;

        // move particle to appropriate child
        int octantForExistingParticle = getOctant(nodes[nodeIdx].regionCenter, existingParticle.position);
        insertParticleRecursive(nodes[nodeIdx].firstChildIdx + octantForExistingParticle, existingParticleIdx, particles);
    }

    // node is not a leaf, insert the new particle into the appropriate child
    int octant = getOctant(nodes[nodeIdx].regionCenter, particle.position);
    insertParticleRecursive(nodes[nodeIdx].firstChildIdx + octant, particleIdx, particles);

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
//    OctreeNode& node = nodes[nodeIdx];

    if (nodes[nodeIdx].firstChildIdx == -1) {
        // empty leaf node, undefined totalMass
        if (nodes[nodeIdx].particleIdx == -1) {
            nodes[nodeIdx].totalMass = 0.0f;
            return;
        }

        // non-empty leaf node, just use the single particle for the
        // center of totalMass and total totalMass
        const Particle& particle = particles[nodes[nodeIdx].particleIdx];
        nodes[nodeIdx].centerOfMass = particle.position;
        nodes[nodeIdx].totalMass = particle.mass;
        return;
    }

    // non-leaf node, need to update totalMass by accounting for all children
    nodes[nodeIdx].totalMass = 0.0f;
    nodes[nodeIdx].centerOfMass = glm::vec3(0.0f);

    for (int i = 0; i < 8; i++) {
        const OctreeNode& child = nodes[nodes[nodeIdx].firstChildIdx + i];
        if (child.totalMass > 0.0f) {
            nodes[nodeIdx].centerOfMass =
                (nodes[nodeIdx].centerOfMass * nodes[nodeIdx].totalMass + child.centerOfMass * child.totalMass) / (nodes[nodeIdx].totalMass + child.totalMass);
            nodes[nodeIdx].totalMass += child.totalMass;
        }
    }
}

const std::vector<OctreeNode>& Octree::getNodes() {
    return nodes;
}

void Octree::calculateForce(int particleIdx, float theta, float softening, const std::vector<Particle>& particles, glm::vec3& force) {
    const Particle& particle = particles[particleIdx];
    calculateForceRecursive(0, particle.position, particle.mass, theta, softening, particles, force);
}

constexpr float G = 6.67430e-12;

void Octree::calculateForceRecursive(int nodeIdx, const glm::vec3& position, float mass, float theta, float softening, const std::vector<Particle>& particles, glm::vec3& force) {
//    const OctreeNode& node = nodes[nodeIdx];

    // node has no particles within and contributes no force, skip
    if (nodes[nodeIdx].totalMass == 0.0f) {
        return;
    }

    // get distance to node's center of mass
    glm::vec3 fromPositionToCenterOfMass = nodes[nodeIdx].centerOfMass - position;
    float distSquared = glm::dot(fromPositionToCenterOfMass, fromPositionToCenterOfMass);

    // avoid calculating forces between a particle and itself
    if (distSquared == 0.0f) {
        return;
    }

    // if the node is an internal node and is far enough away,
    // calculate the force using the internal node's total mass and center of mass
    // (i.e., approximate this group as a single point mass)
    if (nodes[nodeIdx].firstChildIdx == -1 || (nodes[nodeIdx].size * nodes[nodeIdx].size) / distSquared < theta * theta) {
        float distance = std::sqrt(distSquared + softening * softening);
        force += G * mass * nodes[nodeIdx].totalMass * fromPositionToCenterOfMass / (distance * distance * distance);
        return;
    }

    // otherwise, recursively calculate forces from children
    for (int i = 0; i < 8; i++) {
        calculateForceRecursive(nodes[nodeIdx].firstChildIdx + i, position, mass, theta, softening, particles, force);
    }
}

void Octree::clear(const glm::vec3& center, float size) {
    nodes.clear();

    nodes.emplace_back(OctreeNode{
        .centerOfMass = glm::vec3(0.0f),
        .regionCenter = center,
        .size = size,
        .totalMass = 0.0f,
        .firstChildIdx = -1,
        .particleIdx = -1
    });
}

void Octree::drawBoundingBoxes() const {
    for (const OctreeNode& node : nodes) {
        drawBoundingBox(node);
    }
}

void Octree::drawBoundingBox(const OctreeNode& node) const {
    glm::vec3 min = node.regionCenter - glm::vec3(node.size, node.size, node.size) * 0.5f;
    glm::vec3 max = node.regionCenter + glm::vec3(node.size, node.size, node.size) * 0.5f;

    glm::vec3 vertices[] = {
        { min.x, min.y, min.z }, // bottom left (close)
        { max.x, min.y, min.z }, // bottom right (close)
        { max.x, max.y, min.z }, // top right (close)
        { min.x, max.y, min.z }, // top left (close)
        { min.x, min.y, max.z }, // bottom left (far)
        { max.x, min.y, max.z }, // bottom right (far)
        { max.x, max.y, max.z }, // top right (far)
        { min.x, max.y, max.z }  // top left (far)
    };

    size_t indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0, // near face
        4, 5, 5, 6, 6, 7, 7, 4, // far face
        0, 4, 1, 5, 2, 6, 3, 7 // sides
    };

    glBegin(GL_LINES);
    for (size_t i = 0; i < sizeof(indices) / sizeof(size_t); i++) {
        glm::vec3 v = vertices[indices[i]];
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}