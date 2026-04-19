#pragma once
#include <string>
#include <glm/glm.hpp>

class TextRenderer {
public:
    static void init();
    static void print(const std::string& text, float x, float y, float scale = 1.0f, const glm::vec3& color = glm::vec3(1.0f));
    static void printFPS(float fps, float x, float y, float scale = 1.0f);
    static void shutdown();

private:
    static unsigned int VAO, VBO;
    static class Shader* shader;
    static void ensureBufferSize(size_t requiredFloats);
};