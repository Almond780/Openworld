#include "Player.hpp"

Player::Player(const glm::vec3& startPos)
    : body(startPos, 80.0f, glm::vec3(0.5f, 1.0f, 0.5f)), height(2.0f) {}

void Player::takeDamage(int amount) {
    health = std::max(0, health - amount);
    // Optional: add knockback, invincibility frames later
}

void Player::heal(int amount) {
    health = std::min(maxHealth, health + amount);
    damageAccumulator = 0.0f;   // reset DOT buildup
}