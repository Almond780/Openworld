#pragma once
#include <vector>
#include "../core/Mesh.hpp"

class Flattri {
public:
    static Mesh generateHighPoly(float halfSize = 0.5f);
    static Mesh generateMediumPoly(float halfSize = 0.5f);
    static Mesh generateLowPoly(float halfSize = 0.5f);
private:
    static std::vector<float> generateVertices(float halfSize, int subdivisions);
};