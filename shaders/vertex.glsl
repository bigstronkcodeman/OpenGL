#version 430

struct Particle {
    vec4 position;
    vec4 velocity;
    float mass;
    float padding[3];
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

out vec4 velocity;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 pos = particles[gl_VertexID].position;
    gl_Position = projection * view * pos;
    gl_PointSize = particles[gl_VertexID].mass / 100000.0f + 1.0f;
    velocity = particles[gl_VertexID].velocity;
}
