// wall.hpp
#pragma once
#include "../core/Mesh.hpp"

// wall.hpp
class Wall {
public:
    static Mesh generateHighPoly(float halfWidth, float halfHeight, float halfDepth);
    static Mesh generateMediumPoly(float halfWidth, float halfHeight, float halfDepth);
    static Mesh generateLowPoly(float halfWidth, float halfHeight, float halfDepth);
private:
    static std::vector<float> generateVertices(float halfWidth, float halfHeight, float halfDepth);
};