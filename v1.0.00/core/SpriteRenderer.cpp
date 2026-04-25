// core/SpriteRenderer.cpp
#include "SpriteRenderer.hpp"
#include "Shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GLuint SpriteRenderer::VAO = 0;
GLuint SpriteRenderer::VBO = 0;
Shader* SpriteRenderer::shader = nullptr;
std::vector<SpriteRenderer::SpriteVertex> SpriteRenderer::vertices;
bool SpriteRenderer::initialized = false;
glm::mat4 SpriteRenderer::projection = glm::mat4(1.0f);

void SpriteRenderer::init() {
    if (initialized) return;

    // Compile a simple sprite shader (vertex + fragment)
    shader = new Shader("assets/shaders/sprite.vert", "assets/shaders/sprite.frag");
    if (!shader || shader->getID() == 0) {
        std::cerr << "SpriteRenderer: Failed to load sprite shader!" << std::endl;
        return;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Dynamic draw: buffer updated every frame with sprite vertices
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Position attribute (2 floats)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)offsetof(SpriteVertex, x));
    glEnableVertexAttribArray(0);
    // Color attribute (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)offsetof(SpriteVertex, r));
    glEnableVertexAttribArray(1);
    // TexCoord attribute (2 floats)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)offsetof(SpriteVertex, u));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    initialized = true;
    std::cout << "SpriteRenderer initialized." << std::endl;
}

void SpriteRenderer::shutdown() {
    delete shader;
    shader = nullptr;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    initialized = false;
}

void SpriteRenderer::drawRectangle(float x, float y, float width, float height, const glm::vec3& color) {
    // Convert to two triangles (6 vertices)
    float x2 = x + width;
    float y2 = y + height;

    // Triangle 1
    vertices.push_back({x,  y,  color.r, color.g, color.b, 0.0f, 0.0f});
    vertices.push_back({x2, y,  color.r, color.g, color.b, 1.0f, 0.0f});
    vertices.push_back({x2, y2, color.r, color.g, color.b, 1.0f, 1.0f});
    // Triangle 2
    vertices.push_back({x,  y,  color.r, color.g, color.b, 0.0f, 0.0f});
    vertices.push_back({x2, y2, color.r, color.g, color.b, 1.0f, 1.0f});
    vertices.push_back({x,  y2, color.r, color.g, color.b, 0.0f, 1.0f});
}

void SpriteRenderer::flush() {
    if (vertices.empty()) return;
    if (!shader) return;

    // Update projection matrix based on current viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    projection = glm::ortho(0.0f, static_cast<float>(viewport[2]),
                            static_cast<float>(viewport[3]), 0.0f);

    // Upload vertices to GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SpriteVertex),
                 vertices.data(), GL_DYNAMIC_DRAW);

    // Setup shader
    shader->use();
    shader->setMat4("projection", projection);

    // Disable depth testing so sprites always appear on top
    GLboolean depthWasOn = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);

    if (depthWasOn) glEnable(GL_DEPTH_TEST);

    vertices.clear();
}