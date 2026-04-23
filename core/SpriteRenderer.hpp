// core/SpriteRenderer.hpp
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class SpriteRenderer {
public:
    static void init();
    static void shutdown();

    // Draw a colored rectangle (screen coordinates, origin at top-left)
    static void drawRectangle(float x, float y, float width, float height, const glm::vec3& color);

    // Draw a textured rectangle (to be implemented when you add textures)
    // static void drawTexture(float x, float y, float width, float height, GLuint textureID);

    // Call once per frame to flush all sprites
    static void flush();

private:
    struct SpriteVertex {
        float x, y;       // position
        float r, g, b;    // color
        float u, v;       // texcoords (unused for now)
    };

    static GLuint VAO, VBO;
    static class Shader* shader;
    static std::vector<SpriteVertex> vertices;
    static bool initialized;
    static glm::mat4 projection;
};