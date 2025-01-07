#version 430

in vec4 velocity;
in float pointAlpha;

out vec4 FragColor;

const vec3 COLOR_BLUE = vec3(0.4, 0.0, 1.0);
const vec3 COLOR_RED = vec3(0.0, 1.0, 0.25);
const float MAX_SPEED = 0.1;

void main() {
    float speed = length(velocity.xyz);
    float speedFactor = clamp(speed / MAX_SPEED, 0.0, 1.0);
    vec3 finalColor = mix(COLOR_BLUE, COLOR_RED, speedFactor);
    FragColor = vec4(finalColor, pointAlpha);
//    FragColor = vec4(velocity.xyz * 200.0f, pointAlpha);
}
