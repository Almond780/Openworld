#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "objects/Player.hpp"

class FirstPersonCamera {
public:
    FirstPersonCamera(Player* player, const glm::vec3& eyeOffset = glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;
    glm::vec3 getForward() const;
    float getFOV() const { return fov; }

    void processKeyboard(bool keys[], float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void update(float deltaTime);

private:
    float fov;
    float normalFov = 60.0f;   // useless
    float sprintFov = 120.0f;  // useless
    Player* player;
    glm::vec3 eyeOffset;
    float yaw, pitch;
    float movementSpeed;
    float mouseSensitivity;
};