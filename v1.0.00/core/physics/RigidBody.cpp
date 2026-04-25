#include "RigidBody.hpp"

RigidBody::RigidBody(const glm::vec3& pos, float m, const glm::vec3& hExt)
    : position(pos), velocity(0.0f), mass(m), isGrounded(false), halfExtents(hExt) {}

void RigidBody::applyForce(const glm::vec3& force, float deltaTime) {
    if (mass > 0.0f)
        velocity += (force / mass) * deltaTime;
}