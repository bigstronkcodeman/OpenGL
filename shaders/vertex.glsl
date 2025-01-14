#version 430

struct Particle {
    vec4 position;
    vec4 velocity;
    float mass;
    float padding[3];
    vec4 previousPositions[16];
};

layout(std430, binding = 0) buffer ParticleBufferA {
    Particle particlesA[];
};

layout(std430, binding = 1) buffer ParticleBufferB {
    Particle particlesB[];
};

uniform int currentBuffer;
out vec4 velocity;
out float pointAlpha;
uniform mat4 view;
uniform mat4 projection;

void main() {
    int particleIndex = gl_VertexID / 17;
    int trailIndex = gl_VertexID % 17;

    Particle particle = (currentBuffer == 0) ? particlesA[particleIndex] : particlesB[particleIndex];

    vec4 pos;
    if (trailIndex == 0) {
        pos = particle.position;
    } else {
        pos = particle.previousPositions[trailIndex-1];
    }
    pointAlpha = 1.0 - (float(trailIndex) / 17.0);

    gl_Position = projection * view * pos;
    gl_PointSize = 5.0f;

    velocity = particle.velocity;
}
