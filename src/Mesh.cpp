#include "Mesh.h"
#include <iostream>
#include <cmath>

namespace llmvis {

Mesh::Mesh()
    : m_vao(0), m_vbo(0), m_ebo(0)
{
}

Mesh::~Mesh() {
    // Clean up OpenGL resources
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
    }
    
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
    }
    
    if (m_ebo != 0) {
        glDeleteBuffers(1, &m_ebo);
    }
}

bool Mesh::loadFromFile(const std::string& filepath) {
    // This would load a mesh from an OBJ or similar file
    // For simplicity, not implemented in this example
    std::cout << "Loading mesh from file not implemented in this example." << std::endl;
    
    // Instead, create a basic cube
    return false;
}

void Mesh::createSphere(float radius, int subdivisions) {
    // Clear existing data
    m_vertices.clear();
    m_indices.clear();
    
    // Create a sphere using UV-sphere method (stacks and slices)
    const float PI = 3.14159265359f;
    
    // Create vertices
    for (int stack = 0; stack <= subdivisions; ++stack) {
        float stackAngle = PI * (float)stack / subdivisions;
        float stackSin = sin(stackAngle);
        float stackCos = cos(stackAngle);
        
        for (int slice = 0; slice <= subdivisions; ++slice) {
            float sliceAngle = 2.0f * PI * (float)slice / subdivisions;
            float sliceSin = sin(sliceAngle);
            float sliceCos = cos(sliceAngle);
            
            // Position
            float x = radius * stackSin * sliceCos;
            float y = radius * stackCos;
            float z = radius * stackSin * sliceSin;
            
            // Normal (normalized position for sphere)
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            
            // Texture coordinates
            float u = (float)slice / subdivisions;
            float v = (float)stack / subdivisions;
            
            // Add vertex
            Vertex vertex;
            vertex.position = glm::vec3(x, y, z);
            vertex.normal = normal;
            vertex.texCoord = glm::vec2(u, v);
            
            m_vertices.push_back(vertex);
        }
    }
    
    // Create indices
    int sliceCount = subdivisions + 1;
    for (int stack = 0; stack < subdivisions; ++stack) {
        for (int slice = 0; slice < subdivisions; ++slice) {
            int first = stack * sliceCount + slice;
            int second = stack * sliceCount + (slice + 1);
            int third = (stack + 1) * sliceCount + (slice + 1);
            int fourth = (stack + 1) * sliceCount + slice;
            
            // First triangle
            m_indices.push_back(first);
            m_indices.push_back(fourth);
            m_indices.push_back(second);
            
            // Second triangle
            m_indices.push_back(second);
            m_indices.push_back(fourth);
            m_indices.push_back(third);
        }
    }
    
    // Set up mesh
    setupMesh();
}

void Mesh::createCylinder(float radius, float height, int subdivisions) {
    // Clear existing data
    m_vertices.clear();
    m_indices.clear();
    
    const float PI = 3.14159265359f;
    height *= 0.5f; // Half-height for easier centering
    
    // Create vertices for the cylinder body
    for (int i = 0; i <= subdivisions; ++i) {
        float angle = 2.0f * PI * (float)i / subdivisions;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        
        // Bottom vertex
        Vertex bottomVertex;
        bottomVertex.position = glm::vec3(x, -height, z);
        bottomVertex.normal = glm::normalize(glm::vec3(x, 0.0f, z));
        bottomVertex.texCoord = glm::vec2((float)i / subdivisions, 0.0f);
        m_vertices.push_back(bottomVertex);
        
        // Top vertex
        Vertex topVertex;
        topVertex.position = glm::vec3(x, height, z);
        topVertex.normal = glm::normalize(glm::vec3(x, 0.0f, z));
        topVertex.texCoord = glm::vec2((float)i / subdivisions, 1.0f);
        m_vertices.push_back(topVertex);
        
        // Add bottom cap center vertex for the first point
        if (i == 0) {
            Vertex bottomCenter;
            bottomCenter.position = glm::vec3(0.0f, -height, 0.0f);
            bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
            bottomCenter.texCoord = glm::vec2(0.5f, 0.5f);
            m_vertices.push_back(bottomCenter);
            
            Vertex topCenter;
            topCenter.position = glm::vec3(0.0f, height, 0.0f);
            topCenter.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            topCenter.texCoord = glm::vec2(0.5f, 0.5f);
            m_vertices.push_back(topCenter);
        }
    }
    
    // Create indices for the cylinder body
    for (int i = 0; i < subdivisions; ++i) {
        int bottomLeft = i * 2;
        int bottomRight = (i * 2 + 2) % (subdivisions * 2);
        int topLeft = i * 2 + 1;
        int topRight = (i * 2 + 3) % (subdivisions * 2);
        
        // First triangle
        m_indices.push_back(bottomLeft);
        m_indices.push_back(topLeft);
        m_indices.push_back(bottomRight);
        
        // Second triangle
        m_indices.push_back(bottomRight);
        m_indices.push_back(topLeft);
        m_indices.push_back(topRight);
        
        // Bottom cap triangle
        m_indices.push_back(bottomLeft);
        m_indices.push_back(bottomRight);
        m_indices.push_back(subdivisions * 2);
        
        // Top cap triangle
        m_indices.push_back(topLeft);
        m_indices.push_back(subdivisions * 2 + 1);
        m_indices.push_back(topRight);
    }
    
    // Set up mesh
    setupMesh();
}

void Mesh::createQuad(float width, float height) {
    // Clear existing data
    m_vertices.clear();
    m_indices.clear();
    
    // Scale for easier centering
    width *= 0.5f;
    height *= 0.5f;
    
    // Create vertices
    Vertex v1, v2, v3, v4;
    
    // Bottom left
    v1.position = glm::vec3(-width, -height, 0.0f);
    v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v1.texCoord = glm::vec2(0.0f, 0.0f);
    
    // Bottom right
    v2.position = glm::vec3(width, -height, 0.0f);
    v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v2.texCoord = glm::vec2(1.0f, 0.0f);
    
    // Top right
    v3.position = glm::vec3(width, height, 0.0f);
    v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v3.texCoord = glm::vec2(1.0f, 1.0f);
    
    // Top left
    v4.position = glm::vec3(-width, height, 0.0f);
    v4.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    v4.texCoord = glm::vec2(0.0f, 1.0f);
    
    m_vertices.push_back(v1);
    m_vertices.push_back(v2);
    m_vertices.push_back(v3);
    m_vertices.push_back(v4);
    
    // Create indices
    m_indices.push_back(0);
    m_indices.push_back(1);
    m_indices.push_back(2);
    
    m_indices.push_back(0);
    m_indices.push_back(2);
    m_indices.push_back(3);
    
    // Set up mesh
    setupMesh();
}

void Mesh::render() {
    // Bind VAO and draw elements
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::setupMesh() {
    // Clean up any previous VAO/VBO/EBO
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
    }
    
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
    }
    
    if (m_ebo != 0) {
        glDeleteBuffers(1, &m_ebo);
    }
    
    // Create and bind VAO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    
    glBindVertexArray(m_vao);
    
    // Load data into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
    
    // Load data into element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
    
    // Set vertex attribute pointers
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    // TexCoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    
    // Unbind VAO
    glBindVertexArray(0);
}

} // namespace llmvis 