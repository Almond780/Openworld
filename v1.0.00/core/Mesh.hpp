#pragma once
#include <vector>
#include <glad/glad.h>

class Mesh {
public:
    Mesh();
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices = {});
    ~Mesh();

    // Disable copying
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Enable moving
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void setup(const std::vector<float>& vertices, const std::vector<unsigned int>& indices = {});
    void draw() const;

private:
    unsigned int VAO, VBO, EBO;
    int vertexCount;
    bool hasIndices;
};