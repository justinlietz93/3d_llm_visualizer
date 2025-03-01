#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace llmvis {

Shader::Shader() 
    : m_programId(0)
{
}

Shader::~Shader() {
    glDeleteProgram(m_programId);
}

bool Shader::loadFromFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    // Read vertex shader code from file
    std::string vertexCode;
    std::ifstream vShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        vShaderFile.open(vertexShaderPath);
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vShaderFile.close();
        vertexCode = vShaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::VERTEX::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        return false;
    }
    
    // Read fragment shader code from file
    std::string fragmentCode;
    std::ifstream fShaderFile;
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        fShaderFile.open(fragmentShaderPath);
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fShaderFile.close();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FRAGMENT::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        return false;
    }
    
    return loadFromSource(vertexCode, fragmentCode);
}

bool Shader::loadFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    GLuint vertex, fragment;
    
    // Compile vertex shader
    const char* vShaderCode = vertexShaderSource.c_str();
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    if (!checkCompileErrors(vertex, "VERTEX")) {
        return false;
    }
    
    // Compile fragment shader
    const char* fShaderCode = fragmentShaderSource.c_str();
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    if (!checkCompileErrors(fragment, "FRAGMENT")) {
        return false;
    }
    
    // Create shader program
    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertex);
    glAttachShader(m_programId, fragment);
    glLinkProgram(m_programId);
    if (!checkCompileErrors(m_programId, "PROGRAM")) {
        return false;
    }
    
    // Delete shaders as they're linked into the program now and no longer needed
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return true;
}

void Shader::use() {
    glUseProgram(m_programId);
}

void Shader::setUniform(const std::string& name, bool value) {
    GLint location = glGetUniformLocation(m_programId, name.c_str());
    glUniform1i(location, (int)value);
}

void Shader::setUniform(const std::string& name, int value) {
    GLint location = glGetUniformLocation(m_programId, name.c_str());
    glUniform1i(location, value);
}

void Shader::setUniform(const std::string& name, float value) {
    GLint location = glGetUniformLocation(m_programId, name.c_str());
    glUniform1f(location, value);
}

void Shader::setUniform(const std::string& name, const glm::vec2& value) {
    GLint location = glGetUniformLocation(m_programId, name.c_str());
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) {
    GLint location = glGetUniformLocation(m_programId, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::vec4& value) {
    GLint location = glGetUniformLocation(m_programId, name.c_str());
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) {
    GLint location = glGetUniformLocation(m_programId, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

bool Shader::checkCompileErrors(GLuint shader, const std::string& type) {
    int success;
    char infoLog[1024];
    
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    }
    
    return true;
}

} // namespace llmvis 