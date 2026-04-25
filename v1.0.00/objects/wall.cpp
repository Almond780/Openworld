#include "wall.hpp"
#include <cmath>

// wall.cpp
std::vector<float> Wall::generateVertices(float halfWidth, float halfHeight, float halfDepth) {
    float w = halfWidth;
    float h = halfHeight;
    float d = halfDepth;

    std::vector<float> verts = {
        // Back face
        -w, -h, -d,  1.0f, 0.5f, 0.0f,
        w, -h, -d,  1.0f, 0.5f, 0.0f,
        w,  h, -d,  1.0f, 0.5f, 0.0f,
        w,  h, -d,  1.0f, 0.5f, 0.0f,
        -w,  h, -d,  1.0f, 0.5f, 0.0f,
        -w, -h, -d,  1.0f, 0.5f, 0.0f,
        // Front face
        -w, -h,  d,  1.0f, 0.5f, 0.0f,
        w, -h,  d,  1.0f, 0.5f, 0.0f,
        w,  h,  d,  1.0f, 0.5f, 0.0f,
        w,  h,  d,  1.0f, 0.5f, 0.0f,
        -w,  h,  d,  1.0f, 0.5f, 0.0f,
        -w, -h,  d,  1.0f, 0.5f, 0.0f,
        // Left face
        -w,  h,  d,  1.0f, 0.5f, 0.0f,
        -w,  h, -d,  1.0f, 0.5f, 0.0f,
        -w, -h, -d,  1.0f, 0.5f, 0.0f,
        -w, -h, -d,  1.0f, 0.5f, 0.0f,
        -w, -h,  d,  1.0f, 0.5f, 0.0f,
        -w,  h,  d,  1.0f, 0.5f, 0.0f,
        // Right face
        w,  h,  d,  1.0f, 0.5f, 0.0f,
        w,  h, -d,  1.0f, 0.5f, 0.0f,
        w, -h, -d,  1.0f, 0.5f, 0.0f,
        w, -h, -d,  1.0f, 0.5f, 0.0f,
        w, -h,  d,  1.0f, 0.5f, 0.0f,
        w,  h,  d,  1.0f, 0.5f, 0.0f,
        // Bottom face
        -w, -h, -d,  1.0f, 0.5f, 0.0f,
        w, -h, -d,  1.0f, 0.5f, 0.0f,
        w, -h,  d,  1.0f, 0.5f, 0.0f,
        w, -h,  d,  1.0f, 0.5f, 0.0f,
        -w, -h,  d,  1.0f, 0.5f, 0.0f,
        -w, -h, -d,  1.0f, 0.5f, 0.0f,
        // Top face
        -w,  h, -d,  1.0f, 0.5f, 0.0f,
        w,  h, -d,  1.0f, 0.5f, 0.0f,
        w,  h,  d,  1.0f, 0.5f, 0.0f,
        w,  h,  d,  1.0f, 0.5f, 0.0f,
        -w,  h,  d,  1.0f, 0.5f, 0.0f,
        -w,  h, -d,  1.0f, 0.5f, 0.0f
    };
    return verts;
}

Mesh Wall::generateHighPoly(float halfWidth, float halfHeight, float halfDepth) {
    return Mesh(Wall::generateVertices(halfWidth, halfHeight, halfDepth));
}

Mesh Wall::generateMediumPoly(float halfWidth, float halfHeight, float halfDepth) {
    return Mesh(Wall::generateVertices(halfWidth, halfHeight, halfDepth));
}
Mesh Wall::generateLowPoly(float halfWidth, float halfHeight, float halfDepth) {
    return Mesh(Wall::generateVertices(halfWidth, halfHeight, halfDepth));
}