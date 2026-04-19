#include "Player.hpp"

Player::Player(const glm::vec3& startPos)
    : body(startPos, 80.0f, glm::vec3(0.5f, 1.0f, 0.5f)), height(2.0f) {}