#pragma once
#include <glm/glm.hpp>
#include <array>

class Frustum {
public:
    void extractPlanes(const glm::mat4& vp);
    bool isAABBVisible(const glm::vec3& min, const glm::vec3& max) const;
private:
    std::array<glm::vec4, 6> planes;
};