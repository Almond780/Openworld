#include "Frustum.hpp"
#include <glm/gtc/matrix_access.hpp>

void Frustum::extractPlanes(const glm::mat4& vp) {
    // Left plane
    planes[0] = glm::row(vp, 3) + glm::row(vp, 0);
    // Right plane
    planes[1] = glm::row(vp, 3) - glm::row(vp, 0);
    // Bottom plane
    planes[2] = glm::row(vp, 3) + glm::row(vp, 1);
    // Top plane
    planes[3] = glm::row(vp, 3) - glm::row(vp, 1);
    // Near plane
    planes[4] = glm::row(vp, 3) + glm::row(vp, 2);
    // Far plane
    planes[5] = glm::row(vp, 3) - glm::row(vp, 2);

    // Normalize all planes
    for (auto& plane : planes) {
        float len = glm::length(glm::vec3(plane));
        plane /= len;
    }
}

bool Frustum::isAABBVisible(const glm::vec3& min, const glm::vec3& max) const {
    for (const auto& plane : planes) {
        // Compute the positive vertex (the one furthest in the direction of the plane normal)
        glm::vec3 p = min;
        if (plane.x >= 0.0f) p.x = max.x;
        if (plane.y >= 0.0f) p.y = max.y;
        if (plane.z >= 0.0f) p.z = max.z;

        // If the positive vertex is outside the plane, the AABB is outside
        if (glm::dot(glm::vec3(plane), p) + plane.w < 0.0f)
            return false;
    }
    return true;
}