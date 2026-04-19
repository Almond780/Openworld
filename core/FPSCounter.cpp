#include "FPSCounter.hpp"
#include <GLFW/glfw3.h>

FPSCounter::FPSCounter() : lastTime(glfwGetTime()), frameCount(0), currentFPS(0.0f) {}

void FPSCounter::update() {
    double currentTime = glfwGetTime();
    frameCount++;
    if (currentTime - lastTime >= 1.0) {
        currentFPS = static_cast<float>(frameCount) / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
    }
}