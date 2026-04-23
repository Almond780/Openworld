#include "Ramp.hpp"

std::vector<float> Ramp::generateVertices(float halfSize, int subdivisions) {
    // For simplicity, subdivisions is ignored – you can add tessellation later.
    float w = halfSize;        // half width (X)
    float h = halfSize * 2.0f; // height (Y) – makes ramp 2 units tall when halfSize=0.5
    float d = halfSize;        // half depth (Z)

    // A nice orange‑brown color
    float r = 0.8f, g = 0.5f, b = 0.2f;

    std::vector<float> verts = {
        // Bottom face (rectangle at y=0)
        -w, 0.0f, -d,  r, g, b,
        w, 0.0f, -d,  r, g, b,
        w, 0.0f,  d,  r, g, b,
        w, 0.0f,  d,  r, g, b,
        -w, 0.0f,  d,  r, g, b,
        -w, 0.0f, -d,  r, g, b,

        // Back face (rectangle at z = -d, from y=0 to y=h)
        -w, 0.0f, -d,  r, g, b,
        w, 0.0f, -d,  r, g, b,
        w,    h, -d,  r, g, b,
        w,    h, -d,  r, g, b,
        -w,    h, -d,  r, g, b,
        -w, 0.0f, -d,  r, g, b,

        // Slanted top face (the ramp surface)
        -w, 0.0f,  d,  r, g, b,   // front bottom left
        w, 0.0f,  d,  r, g, b,   // front bottom right
        w,    h, -d,  r, g, b,   // back top right
        w,    h, -d,  r, g, b,
        -w,    h, -d,  r, g, b,   // back top left
        -w, 0.0f,  d,  r, g, b,   // front bottom left

        // Left triangular side
        -w, 0.0f,  d,  r, g, b,
        -w, 0.0f, -d,  r, g, b,
        -w,    h, -d,  r, g, b,

        // Right triangular side
        w, 0.0f,  d,  r, g, b,
        w,    h, -d,  r, g, b,
        w, 0.0f, -d,  r, g, b
    };
    return verts;
}

Mesh Ramp::generateHighPoly(float halfSize) {
    return Mesh(Ramp::generateVertices(halfSize, 4));
}
Mesh Ramp::generateMediumPoly(float halfSize) {
    return Mesh(Ramp::generateVertices(halfSize, 2));
}
Mesh Ramp::generateLowPoly(float halfSize) {
    return Mesh(Ramp::generateVertices(halfSize, 1));
}