#include "Flattri.hpp"

std::vector<float> Flattri::generateVertices(float halfSize, int subdivisions) {
    // For simplicity, subdivisions is ignored – you can add tessellation later.
    float w = halfSize;        // half width (X)
    float h = halfSize; // height (Y)
    float d = halfSize;        // half depth (Z)

    // red
    float r = 1.0f, g = 0.0f, b = 0.0f;

    std::vector<float> verts = {
        // positions            // colors
        0.0f,  1.0f, 0.0f,     r, g, b,
        -1.0f, -1.0f, 0.0f,    r, g, b,
        1.0f, -1.0f, 0.0f,     r, g, b,
    };
    return verts;
}

Mesh Flattri::generateHighPoly(float halfSize) {
    return Mesh(Flattri::generateVertices(halfSize, 4));
}
Mesh Flattri::generateMediumPoly(float halfSize) {
    return Mesh(Flattri::generateVertices(halfSize, 2));
}
Mesh Flattri::generateLowPoly(float halfSize) {
    return Mesh(Flattri::generateVertices(halfSize, 1));
}