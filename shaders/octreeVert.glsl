#version 430

layout(location = 0) in vec3 vertexPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(vertexPos, 1.0f);
}
