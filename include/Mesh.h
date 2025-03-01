#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace llmvis {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh {
public:
    Mesh();
    ~Mesh();
    
    bool loadFromFile(const std::string& filepath);
    void createSphere(float radius, int subdivisions);
    void createCylinder(float radius, float height, int subdivisions);
    void createQuad(float width, float height);
    
    void render();
    
private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    
    void setupMesh();
};

} // namespace llmvis 