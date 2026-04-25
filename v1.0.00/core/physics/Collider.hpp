#pragma once
#include <glm/glm.hpp>

struct AABB {
    glm::vec3 min, max;
};

class Collider {
public:
    enum Type { NONE, AABB_TYPE, SPHERE };  // renamed to avoid name clash
    Type type;
    AABB aabb;
    float radius;

    Collider();
    void setAABB(const glm::vec3& min, const glm::vec3& max);
    void setSphere(float r);
};