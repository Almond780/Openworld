
#include "Mesh.hpp"

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), vertexCount(0), hasIndices(false) {}

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    setup(vertices, indices);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

// Move constructor
Mesh::Mesh(Mesh&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
    vertexCount(other.vertexCount), hasIndices(other.hasIndices) {
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.vertexCount = 0;
}

// Move assignment operator
Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);

        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        vertexCount = other.vertexCount;
        hasIndices = other.hasIndices;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.vertexCount = 0;
    }
    return *this;
}

void Mesh::setup(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    hasIndices = !indices.empty();
    vertexCount = hasIndices ? static_cast<int>(indices.size()) : static_cast<int>(vertices.size() / 6);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    if (hasIndices) glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    if (hasIndices) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    // position (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    if (hasIndices) glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
    else glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}