#version 430

in vec4 velocity;

out vec4 FragColor;

const vec3 COLOR_BLUE = vec3(0.0, 0.0, 1.0);
const vec3 COLOR_RED = vec3(1.0, 0.0, 0.0);
const float MAX_SPEED = 0.5;

void main() {
    float speed = length(velocity.xyz);
    float speedFactor = clamp(speed / MAX_SPEED, 0.0, 1.0);
    vec3 finalColor = mix(COLOR_BLUE, COLOR_RED, speedFactor);
    FragColor = vec4(finalColor, 1.0);
//    FragColor = vec4(1.0f);
}
