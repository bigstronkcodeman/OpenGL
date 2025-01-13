#version 430
layout(local_size_x = 1024) in;

struct Particle {
    vec4 position;
    vec4 velocity;
    float mass;
    float padding[3];
    vec4 previousPositions[16];
};

layout(std430, binding = 0) readonly buffer ParticleBufferIn {
    Particle particles_in[];
};

layout(std430, binding = 1) buffer ParticleBufferOut {
    Particle particles_out[];
};

uniform float deltaTime;
const float G = 6.67430e-12;
const float softening = 0.02;
const float drag = 0.001;

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index >= particles_in.length()) return;

    vec3 pos = particles_in[index].position.xyz;
    vec3 vel = particles_in[index].velocity.xyz;
    float mass = particles_in[index].mass;

    vec3 dragForce = -drag * vel;

    vec3 totalForce = vec3(0.0);

    for (uint i = 0; i < particles_in.length(); i++) {
        if (i == index) continue;

        vec3 otherPos = particles_in[i].position.xyz;
        float otherMass = particles_in[i].mass;

        vec3 dir = otherPos - pos;
        float distSqr = dot(dir, dir) + softening;

        totalForce += (normalize(dir) * G * mass * otherMass / distSqr) + dragForce;
    }


    vec3 acc = totalForce / mass;
    vec3 newVel = vel + acc * deltaTime;
    vec3 newPos = pos + newVel * deltaTime;

    particles_out[index].velocity.xyz = newVel;

    for (int i = 15; i > 0; i--) {
        particles_out[index].previousPositions[i] = particles_out[index].previousPositions[i-1];
    }
    particles_out[index].previousPositions[0] = particles_out[index].position;

    particles_out[index].position.xyz = newPos;
}
