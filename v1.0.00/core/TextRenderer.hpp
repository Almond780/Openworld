#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

class TextRenderer {
public:
    static void init();
    static void print(const std::string& text, float x, float y, float scale = 1.0f, const glm::vec3& color = glm::vec3(1.0f));
    static void printFPS(float fps, float x, float y, float scale = 1.0f);
    static void shutdown();

    // Print text at a 3D world position (sign)
    static void print3D(const std::string& text,
                        const glm::vec3& position,
                        const glm::vec3& rotation = glm::vec3(0.0f),   // Euler angles
                        float scale = 1.0f,
                        const glm::vec3& color = glm::vec3(1.0f));

private:
    static unsigned int VAO, VBO;
    static class Shader* shader;
    static void ensureBufferSize(size_t requiredFloats);
    static Shader* shader3D;              // 3D text shader

    // Reusable buffers for 3D labels
    static GLuint labelVAO, labelVBO;
    static GLsizei labelVertexCount;
    static void initLabelBuffers();

};