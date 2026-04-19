//not in use!!!

#include "TextRenderer.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cstdio>
#include <iostream>

#define STB_EASY_FONT_IMPLEMENTATION
#include "core/core/stb_easy_font.h"   // adjust path if needed

unsigned int TextRenderer::VAO = 0;
unsigned int TextRenderer::VBO = 0;
Shader* TextRenderer::shader = nullptr;
static std::vector<float> quadBuffer;
static std::vector<float> triangleBuffer;

void TextRenderer::init() {
    std::cout << "TextRenderer::init() called" << std::endl;

    shader = new Shader("assets/shaders/text/text.vert", "assets/shaders/text/text.frag");

    if (!shader || shader->getID() == 0) {
        std::cerr << "TextRenderer: Failed to load text shader!" << std::endl;
        return;
    }
    std::cout << "Text shader compiled successfully, ID: " << shader->getID() << std::endl;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void TextRenderer::ensureBufferSize(size_t requiredFloats) {
    if (quadBuffer.size() < requiredFloats)
        quadBuffer.resize(requiredFloats);
}

static void convertQuadsToTriangles(const std::vector<float>& quads, int numQuads, std::vector<float>& outTriangles) {
    outTriangles.clear();
    outTriangles.reserve(numQuads * 6 * 2);
    const float* src = quads.data();
    for (int i = 0; i < numQuads; ++i) {
        float v0x = src[0], v0y = src[1];
        float v1x = src[4], v1y = src[5];
        float v2x = src[8], v2y = src[9];
        float v3x = src[12], v3y = src[13];

        outTriangles.push_back(v0x); outTriangles.push_back(v0y);
        outTriangles.push_back(v1x); outTriangles.push_back(v1y);
        outTriangles.push_back(v2x); outTriangles.push_back(v2y);

        outTriangles.push_back(v0x); outTriangles.push_back(v0y);
        outTriangles.push_back(v2x); outTriangles.push_back(v2y);
        outTriangles.push_back(v3x); outTriangles.push_back(v3y);

        src += 16;
    }
}

void TextRenderer::print(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    static bool firstCall = true;
    if (firstCall) {
        std::cout << "Hardcoded rectangle test with text shader" << std::endl;
        firstCall = false;
    }

    if (!shader) return;

    // Hardcoded quad vertices (x,y)
    float vertices[] = {
        100.0f, 100.0f,   // v0
        300.0f, 100.0f,   // v1
        300.0f, 200.0f,   // v2
        100.0f, 100.0f,   // v0
        300.0f, 200.0f,   // v2
        100.0f, 200.0f    // v3
    };

    // Upload to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Ortho projection
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(viewport[2]), 0.0f, static_cast<float>(viewport[3]));

    // Set state
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    shader->use();
    //GLint posLoc = glGetAttribLocation(shader->getID(), "aPos");
    //std::cout << "aPos location: " << posLoc << std::endl;
    //if (posLoc != 0) std::cerr << "WARNING: aPos not at location 0!" << std::endl;

    // Check current VAO binding
    //GLint currentVAO;
    //glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    //std::cout << "Current VAO: " << currentVAO << std::endl;
    shader->setMat4("projection", projection);
    shader->setVec3("textColor", glm::vec3(1.0f, 0.0f, 0.0f));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << std::hex << err << std::dec << std::endl;
    }

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void TextRenderer::printFPS(float fps, float x, float y, float scale) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "FPS: %.1f", fps);
    print(buffer, x, y, scale, glm::vec3(1.0f, 1.0f, 0.0f));
}

void TextRenderer::shutdown() {
    std::cout << "TextRenderer::shutdown() called" << std::endl;
    delete shader;
    shader = nullptr;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}