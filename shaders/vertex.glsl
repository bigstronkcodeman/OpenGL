#version 430

struct Particle {
    vec4 position;
    vec4 velocity;
    float mass;
    float padding[3];
    vec4 previousPositions[16];
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

out vec4 velocity;
out float pointAlpha;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
    int particleIndex = gl_VertexID / 17;
    int trailIndex = gl_VertexID % 17;

    vec4 pos;
    if (trailIndex == 0) {
        pos = particles[particleIndex].position;
        pointAlpha = 1.0;
    } else {
        pos = particles[particleIndex].previousPositions[trailIndex-1];
        pointAlpha = 1.0 - (float(trailIndex) / 17.0);
    }

    gl_Position = projection * view * pos;

    float baseSize = particles[particleIndex].mass / 100000.0 + 1.0;
    gl_PointSize = baseSize * (trailIndex == 0 ? 1.0 : (1.0 - float(trailIndex) / 17.0));

    velocity = particles[particleIndex].velocity;
}
