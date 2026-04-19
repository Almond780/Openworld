#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

class Shader {
public:

    // Older: without geometry shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // Newer: with geometry shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);

    ~Shader();
    void use() const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setFloat(const std::string& name, float value) const;   // <-- NEW
    unsigned int getID() const { return ID; }

private:
    unsigned int ID;
    void checkCompileErrors(unsigned int shader, std::string type);
};