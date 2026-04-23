#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>

#include "../core/Mesh.hpp"

//enum class TriggerAction { NONE, TELEPORT, PRINT_MESSAGE }; // optional

class Player;

class GameObject {
public:
    glm::vec3 position;
    std::vector<std::shared_ptr<Mesh>> lodMeshes;
    bool hasCollision = true;
    glm::vec3 halfExtents = glm::vec3(0.5f);          // VISUAL scale (mesh scaling)
    glm::vec3 colliderHalfExtents = glm::vec3(0.5f);  // COLLIDER size
    glm::vec3 rotation = glm::vec3(0.0f);

    // Trigger system
    bool isTrigger = false;
    bool triggerFired = false;
    std::function<void(class Player&)> onTriggerEnter;
    void draw(int lodLevel) const;
    bool isAnimated = false;
    bool isOpen = false;
    float openAngle = 90.0f;          // degrees to rotate when open
    float animationSpeed = 2.0f;      // rotation speed factor
    float currentRotation = 0.0f;     // current rotation in degrees
    glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f); // hinge axis
    glm::vec3 hingeOffset = glm::vec3(0.0f); // offset from position (if needed)

    // Moving platform
    bool isMoving = false;
    glm::vec3 startPos;
    glm::vec3 endPos;
    float moveSpeed = 1.0f;      // units per second
    float moveT = 0.0f;          // interpolation factor 0..1
    int moveDirection = 1;       // 1 = forward, -1 = backward
};