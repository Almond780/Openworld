#include "FirstPersonCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

FirstPersonCamera::FirstPersonCamera(Player* p, const glm::vec3& offset)
    : player(p), eyeOffset(offset), fov(60.0f), yaw(-90.0f), pitch(0.0f),
    movementSpeed(5.0f), mouseSensitivity(0.1f),
    normalFov(90.0f), sprintFov(120.0f) {}

glm::mat4 FirstPersonCamera::getViewMatrix() const {
    glm::vec3 pos = player->getPosition() + eyeOffset;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    return glm::lookAt(pos, pos + front, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 FirstPersonCamera::getPosition() const {
    return player->getPosition() + eyeOffset;
}

glm::vec3 FirstPersonCamera::getForward() const {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::normalize(front);
}

void FirstPersonCamera::processKeyboard(bool keys[], float deltaTime) {
    glm::vec3 front = getForward();
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 move(0.0f);
    if (keys[GLFW_KEY_W]) move += front;
    if (keys[GLFW_KEY_S]) move -= front;
    if (keys[GLFW_KEY_A]) move -= right;
    if (keys[GLFW_KEY_D]) move += right;
    if (glm::length(move) > 0.0f) move = glm::normalize(move);

    // Sprint FOV and speed
    bool sprinting = (keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]);
    fov = sprinting ? sprintFov : normalFov;
    float speed = sprinting ? movementSpeed * 2.0f : movementSpeed;

    player->getRigidBody()->velocity.x = move.x * speed;
    player->getRigidBody()->velocity.z = move.z * speed;
}

void FirstPersonCamera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    yaw += xoffset;
    pitch += yoffset;
    if (constrainPitch) {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
}

void FirstPersonCamera::update(float deltaTime) {
    // Nothing needed; camera reads player position directly
}