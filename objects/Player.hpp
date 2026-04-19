#pragma once
#include <glm/glm.hpp>
#include "../core/physics/RigidBody.hpp"

class Player {
public:
    Player(const glm::vec3& startPos);
    glm::vec3 getPosition() const { return body.position; }
    RigidBody* getRigidBody() { return &body; }
    float getHeight() const { return height; }

private:
    RigidBody body;
    float height; // 2.0 units
};