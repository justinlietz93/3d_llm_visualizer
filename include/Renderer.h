#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"

namespace llmvis {

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(int width, int height);
    void beginFrame();
    void endFrame();
    
    void setCamera(Camera* camera);
    GLFWwindow* getWindow() const { return m_window; }
    
    void renderLayer(const class Layer* layer);
    void renderNeuron(const glm::vec3& position, float size, const glm::vec4& color);
    void renderConnection(const glm::vec3& from, const glm::vec3& to, float strength, const glm::vec4& color);
    void renderDataFlow(const glm::vec3& start, const glm::vec3& end, float progress, const glm::vec4& color);
    void renderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color);
    void renderRect(float x, float y, float width, float height, const glm::vec4& color);
    
private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    
    std::unique_ptr<Shader> m_neuronShader;
    std::unique_ptr<Shader> m_connectionShader;
    std::unique_ptr<Shader> m_textShader;
    std::unique_ptr<Shader> m_dataFlowShader;
    
    std::unique_ptr<Mesh> m_sphereMesh;
    std::unique_ptr<Mesh> m_cylinderMesh;
    std::unique_ptr<Mesh> m_quadMesh;
    
    Camera* m_camera;
    
    unsigned int m_fontTexture;
    
    // For text rendering
    GLuint m_textVBO;
    GLuint m_textVAO;
    
    void loadShaders();
    void createMeshes();
    void loadFonts();
};

} // namespace llmvis 