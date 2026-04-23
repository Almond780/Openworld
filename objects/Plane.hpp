#pragma once
#include "../core/Mesh.hpp"

class Plane {
public:
    Plane(float width, float depth);
    void draw() const { mesh.draw(); }
private:
    Mesh mesh;
};