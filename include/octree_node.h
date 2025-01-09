//
// Created by prbou on 1/6/2025.
//

#ifndef OPENGL_FUN_OCTREE_NODE_H
#define OPENGL_FUN_OCTREE_NODE_H

#include <glm/glm.hpp>
#include <cstddef>

struct OctreeNode {
    glm::vec3 centerOfMass;
    glm::vec3 regionCenter;
    float size;
    float totalMass;
    int firstChildIdx;
    int particleIdx;
};

#endif //OPENGL_FUN_OCTREE_NODE_H
