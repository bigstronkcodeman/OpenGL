#version 420
out vec4 FragColor;
in vec3 ourColor;

uniform float someUniform;

void main() {
   FragColor = vec4(cos(someUniform) / 2.0f, tan(someUniform) / 2.0f, sin(someUniform) / 2.0f, 1.0f);
}