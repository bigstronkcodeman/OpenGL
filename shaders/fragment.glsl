#version 430

in vec4 velocity;

out vec4 FragColor;

void main() {
//    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 normalizedVelocity = normalize(velocity);
    FragColor = vec4(normalizedVelocity.x, normalizedVelocity.y, normalizedVelocity.z, 1.0f) * 20.0f;
}
