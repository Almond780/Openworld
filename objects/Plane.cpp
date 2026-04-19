#include "Plane.hpp"

Plane::Plane(float width, float depth) {
    float w2 = width * 0.5f;
    float d2 = depth * 0.5f;
    std::vector<float> vertices = {
        // positions          // colors (cyan)
        -w2, 0.0f, -d2,   0.0f, 1.0f, 1.0f,
        w2, 0.0f, -d2,   0.0f, 1.0f, 1.0f,
        w2, 0.0f,  d2,   0.0f, 1.0f, 1.0f,
        -w2, 0.0f, -d2,   0.0f, 1.0f, 1.0f,
        w2, 0.0f,  d2,   0.0f, 1.0f, 1.0f,
        -w2, 0.0f,  d2,   0.0f, 1.0f, 1.0f,
    };
    mesh.setup(vertices, {});
}