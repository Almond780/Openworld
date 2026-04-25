#pragma once
#include <glm/glm.hpp>
#include "../core/physics/RigidBody.hpp"

class Player {
public:
    Player(const glm::vec3& startPos);
    glm::vec3 getPosition() const { return body.position; }
    RigidBody* getRigidBody() { return &body; }
    float getHeight() const { return height; }

    // Health system
    int getHealth() const { return health; }
    void takeDamage(int amount);
    void heal(int amount);
    bool isDead() const { return health <= 0; }

    // Damage‑over‑time accumulator
    float damageAccumulator = 0.0f;

private:
    RigidBody body;
    float height; // 2.0 units

    int health = 100;
    int maxHealth = 100;
};