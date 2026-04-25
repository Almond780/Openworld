#pragma once
#include <glm/glm.hpp>

class RigidBody {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    float mass;
    bool isGrounded;
    glm::vec3 halfExtents;   // <-- NEW: size of the body for collision

    RigidBody(const glm::vec3& pos = glm::vec3(0.0f), float m = 1.0f, const glm::vec3& hExt = glm::vec3(0.5f));
    void applyForce(const glm::vec3& force, float deltaTime);
};