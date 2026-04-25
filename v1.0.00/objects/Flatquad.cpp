#include "Flatquad.hpp"

std::vector<float> Flatquad::generateVertices(float halfSize, int subdivisions) {
    // For simplicity, subdivisions is ignored – you can add tessellation later.
    float w = halfSize;        // half width (X)
    float h = halfSize; // height (Y)
    float d = halfSize;        // half depth (Z)

    // red
    float r = 1.0f, g = 0.0f, b = 0.0f;

    std::vector<float> verts = {
        // first triangle
        -w, -h, 0.0f,   r, g, b,
        w, -h, 0.0f,   r, g, b,
        w,  h, 0.0f,   r, g, b,
        // second triangle
        -w, -h, 0.0f,   r, g, b,
        w,  h, 0.0f,   r, g, b,
        -w,  h, 0.0f,   r, g, b,
    };
    return verts;
}

Mesh Flatquad::generateHighPoly(float halfSize) {
    return Mesh(Flatquad::generateVertices(halfSize, 4));
}
Mesh Flatquad::generateMediumPoly(float halfSize) {
    return Mesh(Flatquad::generateVertices(halfSize, 2));
}
Mesh Flatquad::generateLowPoly(float halfSize) {
    return Mesh(Flatquad::generateVertices(halfSize, 1));
}