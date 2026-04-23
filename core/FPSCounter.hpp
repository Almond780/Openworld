#pragma once

class FPSCounter {
public:
    FPSCounter();
    void update();
    float getCurrentFPS() const { return currentFPS; }
    int getFrameCount() const { return frameCount; }
private:
    double lastTime;
    int frameCount;
    float currentFPS;
};