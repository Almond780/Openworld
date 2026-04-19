#include "Collider.hpp"

Collider::Collider() : type(NONE), radius(0.0f) {}

void Collider::setAABB(const glm::vec3& min, const glm::vec3& max) {
    type = AABB_TYPE;
    aabb.min = min;
    aabb.max = max;
}

void Collider::setSphere(float r) {
    type = SPHERE;
    radius = r;
}

