
#include "TextRenderer.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cstdio>
#include <iostream>

#define STB_EASY_FONT_IMPLEMENTATION
#include "core/core/stb_easy_font.h"   // adjust path to match your actual location

unsigned int TextRenderer::VAO = 0;
unsigned int TextRenderer::VBO = 0;
Shader* TextRenderer::shader = nullptr;
Shader* TextRenderer::shader3D = nullptr;
GLuint TextRenderer::labelVAO = 0;
GLuint TextRenderer::labelVBO = 0;
GLsizei TextRenderer::labelVertexCount = 0;

// Buffers for font data
static std::vector<float> quadBuffer;   // raw quads from stb_easy_font
static std::vector<float> triBuffer;    // converted triangles

extern glm::mat4 g_viewMatrix;
extern glm::mat4 g_projectionMatrix;


void TextRenderer::init() {
    std::cout << "Loading text shader..." << std::endl;
    shader = new Shader("assets/shaders/text/text.vert", "assets/shaders/text/text.frag");

    if (!shader || shader->getID() == 0) {
        std::cerr << "TextRenderer: Failed to load text shader!" << std::endl;
        return;
    }
    //std::cout << "Text shader compiled successfully, ID: " << shader->getID() << std::endl;
    shader3D = new Shader("assets/shaders/text/text3d.vert",
                          "assets/shaders/text/text3d.frag");
    if (!shader3D || shader3D->getID() == 0) {
        std::cerr << "TextRenderer: 3D shader failed to load!" << std::endl;
        delete shader3D;
        shader3D = nullptr;
    } else {
        initLabelBuffers();   // <-- set up the reusable buffers
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    // Check for OpenGL errors after setup
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        //std::cerr << "OpenGL error during TextRenderer init: 0x" << std::hex << err << std::dec << std::endl;
    }

    std::cout << "TextRenderer initialized successfully." << std::endl;

}

void TextRenderer::shutdown() {
    delete shader;
    shader = nullptr;
    delete shader3D;
    shader3D = nullptr;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &labelVAO);
    glDeleteBuffers(1, &labelVBO);
    labelVAO = labelVBO = 0;
}

// Convert stb_easy_font quads (4 vertices per quad) to triangles (6 vertices per quad)
static void convertQuadsToTriangles(const std::vector<float>& quads, int numQuads, std::vector<float>& outTriangles) {
    outTriangles.clear();
    outTriangles.reserve(numQuads * 6 * 2); // 6 vertices * 2 floats each

    const float* src = quads.data();
    for (int i = 0; i < numQuads; ++i) {
        // stb_easy_font vertex layout: x, y, z, u (4 floats per vertex). We only need x,y.
        float v0x = src[0], v0y = src[1];
        float v1x = src[4], v1y = src[5];
        float v2x = src[8], v2y = src[9];
        float v3x = src[12], v3y = src[13];

        // Triangle 1: v0, v1, v2
        outTriangles.push_back(v0x); outTriangles.push_back(v0y);
        outTriangles.push_back(v1x); outTriangles.push_back(v1y);
        outTriangles.push_back(v2x); outTriangles.push_back(v2y);

        // Triangle 2: v0, v2, v3
        outTriangles.push_back(v0x); outTriangles.push_back(v0y);
        outTriangles.push_back(v2x); outTriangles.push_back(v2y);
        outTriangles.push_back(v3x); outTriangles.push_back(v3y);

        src += 16;
    }
}

void TextRenderer::print(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    if (!shader) return;
    if (text.empty()) return;

    size_t maxQuads = text.length() * 150;
    quadBuffer.resize(maxQuads * 16);
    int numQuads = stb_easy_font_print(x, y, const_cast<char*>(text.c_str()), nullptr,
                                       quadBuffer.data(), static_cast<int>(quadBuffer.size()));
    if (numQuads == 0) return;

    convertQuadsToTriangles(quadBuffer, numQuads, triBuffer);


    if (scale != 1.0f) {
        for (size_t i = 0; i < triBuffer.size(); i += 2) {
            // Scale around (x, y) – subtract anchor, scale, add back
            triBuffer[i]     = x + (triBuffer[i]     - x) * scale;
            triBuffer[i + 1] = y + (triBuffer[i + 1] - y) * scale;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, triBuffer.size() * sizeof(float), triBuffer.data(), GL_DYNAMIC_DRAW);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(viewport[2]), static_cast<float>(viewport[3]), 0.0f);

    GLboolean depthWasOn = glIsEnabled(GL_DEPTH_TEST);
    GLboolean cullWasOn = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    shader->use();
    shader->setMat4("projection", proj);
    shader->setVec3("textColor", color);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(triBuffer.size() / 2));
    glBindVertexArray(0);

    if (depthWasOn) glEnable(GL_DEPTH_TEST);
    if (cullWasOn) glEnable(GL_CULL_FACE);
}

void TextRenderer::printFPS(float fps, float x, float y, float scale) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "FPS: %.1f", fps);
    print(buffer, x, y, scale, glm::vec3(1.0f, 1.0f, 0.0f)); // yellow
}

void TextRenderer::print3D(const std::string& text,
                           const glm::vec3& position,
                           const glm::vec3& rotation,
                           float scale,
                           const glm::vec3& color) {
    if (!shader3D || text.empty()) return;
    if (labelVAO == 0) return;   // not initialised

    // Generate the triangle geometry (same as before)
    size_t maxQuads = text.length() * 150;
    quadBuffer.resize(maxQuads * 16);
    int numQuads = stb_easy_font_print(0.0f, 0.0f, const_cast<char*>(text.c_str()),
                                       nullptr, quadBuffer.data(),
                                       static_cast<int>(quadBuffer.size()));
    if (numQuads == 0) return;

    convertQuadsToTriangles(quadBuffer, numQuads, triBuffer);
    if (triBuffer.empty()) return;

    // Apply scale
    if (scale != 1.0f) {
        for (size_t i = 0; i < triBuffer.size(); i += 2) {
            triBuffer[i]     *= scale;
            triBuffer[i + 1] *= scale;
        }
    }

    // Upload to the existing shared VBO
    glBindVertexArray(labelVAO);
    glBindBuffer(GL_ARRAY_BUFFER, labelVBO);
    glBufferData(GL_ARRAY_BUFFER, triBuffer.size() * sizeof(float),
                 triBuffer.data(), GL_DYNAMIC_DRAW);
    // (If you want to be even gentler on the GPU, use glBufferSubData with an approximate size, but glBufferData is fine for now.)

    // Model matrix (same as before)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    if (rotation != glm::vec3(0.0f)) {
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1,0,0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0,1,0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0,0,1));
    }

    shader3D->use();
    shader3D->setMat4("model", model);
    shader3D->setMat4("view", g_viewMatrix);
    shader3D->setMat4("projection", g_projectionMatrix);
    shader3D->setVec3("textColor", color);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(triBuffer.size() / 2));
    glBindVertexArray(0);
}

void TextRenderer::initLabelBuffers() {
    glGenVertexArrays(1, &labelVAO);
    glGenBuffers(1, &labelVBO);
    // We'll set up the vertex attributes once; the VBO will be updated dynamically.
    glBindVertexArray(labelVAO);
    glBindBuffer(GL_ARRAY_BUFFER, labelVBO);
    // Start with an empty buffer; it will be resized later by glBufferData
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}