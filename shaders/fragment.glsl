#version 430

in vec4 velocity;
in float pointAlpha;

out vec4 FragColor;

const vec3 COLOR_BLUE = vec3(0.4, 0.0, 1.0);
const vec3 COLOR_RED = vec3(0.0, 1.0, 0.25);
const float MAX_SPEED = 0.1;

void main() {
//    float speed = length(velocity.xyz);
//    float speedFactor = clamp(speed / MAX_SPEED, 0.0, 1.0);
//    vec3 finalColor = mix(COLOR_BLUE, COLOR_RED, speedFactor);
//    FragColor = vec4(finalColor, pointAlpha);
//    FragColor = vec4(max(velocity.x * 200.0f, 0.2f), max(velocity.y * 200.0f, 0.2f), max(velocity.z * 200.0f, 0.2f), pointAlpha);
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);


//    float r = dot(coord, coord);
//    if (r > 1.0) discard;
//
//    vec3 normal = vec3(coord, sqrt(1.0 - r));
//    vec3 light = normalize(vec3(1.0, 1.0, 1.0));
//    float diffuse = max(dot(normal, light), 0.0);
//
//    FragColor = vec4(vec3(0.8 * diffuse + 0.2), 1.0);
}
