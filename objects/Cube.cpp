#include "Cube.hpp"
#include <cmath>

// Full implementation in Cube.cpp
std::vector<float> Cube::generateVertices(float halfSize, int subdivisions) {

    float s = halfSize;
    std::vector<float> verts = {
        // Back face
        -s, -s, -s,  1.0f, 0.5f, 0.0f,
        s, -s, -s,  1.0f, 0.5f, 0.0f,
        s,  s, -s,  1.0f, 0.5f, 0.0f,
        s,  s, -s,  1.0f, 0.5f, 0.0f,
        -s,  s, -s,  1.0f, 0.5f, 0.0f,
        -s, -s, -s,  1.0f, 0.5f, 0.0f,
        // Front face
        -s, -s,  s,  1.0f, 0.5f, 0.0f,
        s, -s,  s,  1.0f, 0.5f, 0.0f,
        s,  s,  s,  1.0f, 0.5f, 0.0f,
        s,  s,  s,  1.0f, 0.5f, 0.0f,
        -s,  s,  s,  1.0f, 0.5f, 0.0f,
        -s, -s,  s,  1.0f, 0.5f, 0.0f,
        // Left face
        -s,  s,  s,  1.0f, 0.5f, 0.0f,
        -s,  s, -s,  1.0f, 0.5f, 0.0f,
        -s, -s, -s,  1.0f, 0.5f, 0.0f,
        -s, -s, -s,  1.0f, 0.5f, 0.0f,
        -s, -s,  s,  1.0f, 0.5f, 0.0f,
        -s,  s,  s,  1.0f, 0.5f, 0.0f,
        // Right face
        s,  s,  s,  1.0f, 0.5f, 0.0f,
        s,  s, -s,  1.0f, 0.5f, 0.0f,
        s, -s, -s,  1.0f, 0.5f, 0.0f,
        s, -s, -s,  1.0f, 0.5f, 0.0f,
        s, -s,  s,  1.0f, 0.5f, 0.0f,
        s,  s,  s,  1.0f, 0.5f, 0.0f,
        // Bottom face
        -s, -s, -s,  1.0f, 0.5f, 0.0f,
        s, -s, -s,  1.0f, 0.5f, 0.0f,
        s, -s,  s,  1.0f, 0.5f, 0.0f,
        s, -s,  s,  1.0f, 0.5f, 0.0f,
        -s, -s,  s,  1.0f, 0.5f, 0.0f,
        -s, -s, -s,  1.0f, 0.5f, 0.0f,
        // Top face
        -s,  s, -s,  1.0f, 0.5f, 0.0f,
        s,  s, -s,  1.0f, 0.5f, 0.0f,
        s,  s,  s,  1.0f, 0.5f, 0.0f,
        s,  s,  s,  1.0f, 0.5f, 0.0f,
        -s,  s,  s,  1.0f, 0.5f, 0.0f,
        -s,  s, -s,  1.0f, 0.5f, 0.0f
    };
    return verts;
}

Mesh Cube::generateHighPoly(float halfSize) {
    return Mesh(Cube::generateVertices(halfSize, 4));
}
Mesh Cube::generateMediumPoly(float halfSize) {
    return Mesh(Cube::generateVertices(halfSize, 2));
}
Mesh Cube::generateLowPoly(float halfSize) {
    return Mesh(Cube::generateVertices(halfSize, 1));
}