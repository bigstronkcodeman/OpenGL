#version 430
layout(local_size_x = 128) in;

struct Particle {
    vec4 position;
    vec4 velocity;
    float mass;
    float padding[3];
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

uniform float deltaTime;
const float G = 6.67430e-11;
//const float G = 0.0000000002;
const float softening = 0.1;
const float drag = 0.1;

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index >= particles.length()) return;

    vec3 pos = particles[index].position.xyz;
    vec3 vel = particles[index].velocity.xyz;
    float mass = particles[index].mass;

    vec3 dragForce = -drag *  vel;

    vec3 totalForce = vec3(0.0);

    for (uint i = 0; i < particles.length(); i++) {
        if (i == index) continue;

        vec3 otherPos = particles[i].position.xyz;
        float otherMass = particles[i].mass;

        vec3 dir = otherPos - pos;
        float distSqr = dot(dir, dir) + softening;

        totalForce += normalize(dir) * G * mass * otherMass / distSqr;
        totalForce += dragForce;
    }


    vec3 acc = totalForce / mass;
    vec3 newVel = vel + acc * deltaTime;
    vec3 newPos = pos + newVel * deltaTime;

    particles[index].velocity.xyz = newVel;
    particles[index].position.xyz = newPos;
}
