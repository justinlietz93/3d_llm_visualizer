#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace llmvis {

class Shader {
public:
    Shader();
    ~Shader();
    
    bool loadFromFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    bool loadFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    
    void use();
    void setUniform(const std::string& name, bool value);
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, const glm::vec2& value);
    void setUniform(const std::string& name, const glm::vec3& value);
    void setUniform(const std::string& name, const glm::vec4& value);
    void setUniform(const std::string& name, const glm::mat4& value);
    
private:
    GLuint m_programId;
    
    bool checkCompileErrors(GLuint shader, const std::string& type);
};

} // namespace llmvis 