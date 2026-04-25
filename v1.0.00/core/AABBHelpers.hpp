#pragma once
#include <glm/glm.hpp>
#include "physics/Collider.hpp" // for AABB
#include <glm/gtc/matrix_transform.hpp>

// ------------------------------------------------------------------
// AABB Helper Functions
// ------------------------------------------------------------------

// Transform an AABB by a rotation matrix (no translation) – returns an AABB that encloses the rotated box
inline AABB transformAABB(const AABB& aabb, const glm::mat3& rotation) {
    // Compute the 8 corners of the AABB
    glm::vec3 corners[8] = {
        glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.max.y, aabb.max.z)
    };
    // Rotate all corners and find new min/max
    AABB result;
    result.min = glm::vec3(FLT_MAX);
    result.max = glm::vec3(-FLT_MAX);
    for (int i = 0; i < 8; ++i) {
        glm::vec3 rotated = rotation * corners[i];
        result.min = glm::min(result.min, rotated);
        result.max = glm::max(result.max, rotated);
    }
    return result;
}

// Simple overlap test (no correction)
inline bool checkAABBOverlap(const AABB& a, const AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

inline bool AABBvsAABB(const AABB& a, const AABB& b, glm::vec3& correction) {
    float dx = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    float dy = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    float dz = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);

    if (dx > 0.0f && dy > 0.0f && dz > 0.0f) {
        // Choose the axis with the smallest overlap
        if (dx < dy && dx < dz) {
            correction = glm::vec3((a.max.x > b.max.x) ? dx : -dx, 0.0f, 0.0f);
        } else if (dy < dx && dy < dz) {
            correction = glm::vec3(0.0f, (a.max.y > b.max.y) ? dy : -dy, 0.0f);
        } else {
            correction = glm::vec3(0.0f, 0.0f, (a.max.z > b.max.z) ? dz : -dz);
        }

        // Ensure correction is at least 0.001 units to avoid zero vectors
        float len = glm::length(correction);
        if (len < 0.001f) {
            // Force a minimal push along the shallowest axis
            if (dx < dy && dx < dz)
                correction.x = (a.max.x > b.max.x) ? 0.001f : -0.001f;
            else if (dy < dx && dy < dz)
                correction.y = (a.max.y > b.max.y) ? 0.001f : -0.001f;
            else
                correction.z = (a.max.z > b.max.z) ? 0.001f : -0.001f;
        }
        return true;
    }
    correction = glm::vec3(0.0f);
    return false;
}