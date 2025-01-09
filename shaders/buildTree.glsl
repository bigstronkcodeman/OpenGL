#version 430

layout(local_size_x = 128) in;

struct OctreeNode {
    vec4 centerOfMass;
    vec4 boundingBoxMin;
    vec4 boundingBoxMax;
    int firstChild;
    int firstParticle;
    float size;
    float padding;
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

layout(std430, binding = 1) buffer OctreeBuffer {
    OctreeNode nodes[];
};

void main() {

}