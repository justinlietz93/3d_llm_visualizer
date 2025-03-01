#include "Renderer.h"
#include "Layer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace llmvis {

Renderer::Renderer()
    : m_window(nullptr)
    , m_width(0)
    , m_height(0)
    , m_fontTexture(0)
{
}

Renderer::~Renderer() {
    // Delete text rendering resources
    if (m_textVAO) {
        glDeleteVertexArrays(1, &m_textVAO);
        m_textVAO = 0;
    }
    
    if (m_textVBO) {
        glDeleteBuffers(1, &m_textVBO);
        m_textVBO = 0;
    }
    
    if (m_fontTexture) {
        glDeleteTextures(1, &m_fontTexture);
        m_fontTexture = 0;
    }
    
    // Release mesh resources
    m_sphereMesh.reset();
    m_cylinderMesh.reset();
    m_quadMesh.reset();
    
    // Release shader resources
    m_neuronShader.reset();
    m_connectionShader.reset();
    m_textShader.reset();
    m_dataFlowShader.reset();
    
    // Destroy window and terminate GLFW
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    
    // Note: Only call glfwTerminate() once at the end of the application
    // It's better to let main() handle this
    
    std::cout << "Renderer resources cleaned up successfully" << std::endl;
}

bool Renderer::initialize(int width, int height) {
    m_width = width;
    m_height = height;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    m_window = glfwCreateWindow(width, height, "LLM Visualization", NULL, NULL);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // Configure OpenGL
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Load shaders
    loadShaders();
    
    // Create meshes
    createMeshes();
    
    // Load fonts
    loadFonts();
    
    return true;
}

void Renderer::beginFrame() {
    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up projection and view matrices if camera is available
    if (m_camera) {
        // Calculate aspect ratio
        float aspectRatio = (float)m_width / (float)m_height;
        
        // Get matrices from camera
        glm::mat4 projection = m_camera->getProjectionMatrix(aspectRatio);
        glm::mat4 view = m_camera->getViewMatrix();
        
        // Apply to ALL shaders
        if (m_neuronShader) {
            m_neuronShader->use();
            m_neuronShader->setUniform("projection", projection);
            m_neuronShader->setUniform("view", view);
            m_neuronShader->setUniform("viewPos", m_camera->getPosition());
            m_neuronShader->setUniform("lightPos", m_camera->getPosition());
        }
        
        if (m_connectionShader) {
            m_connectionShader->use();
            m_connectionShader->setUniform("projection", projection);
            m_connectionShader->setUniform("view", view);
        }
        
        if (m_textShader) {
            m_textShader->use();
            m_textShader->setUniform("projection", projection);
        }
        
        if (m_dataFlowShader) {
            m_dataFlowShader->use();
            m_dataFlowShader->setUniform("projection", projection);
            m_dataFlowShader->setUniform("view", view);
        }
    }
}

void Renderer::endFrame() {
    // Swap buffers
    glfwSwapBuffers(m_window);
    
    // Poll events
    glfwPollEvents();
}

void Renderer::renderLayer(const Layer* layer) {
    // Implementation depends on layer type
    switch (layer->getType()) {
        case LayerType::EMBEDDING:
            // Render as a grid or panel
            {
                glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // Would come from layer
                glm::vec4 color = glm::vec4(0.2f, 0.6f, 0.8f, 0.7f); // Blue for embedding
                
                // Calculate dimensions based on layer size
                float width = 2.0f;
                float height = 2.0f;
                
                // Create model matrix
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, position);
                model = glm::scale(model, glm::vec3(width, height, 0.1f));
                
                // Use appropriate shader
                m_neuronShader->use();
                m_neuronShader->setUniform("model", model);
                m_neuronShader->setUniform("color", color);
                
                // Render a quad
                m_quadMesh->render();
            }
            break;
            
        case LayerType::NORMALIZATION:
            // Render as a thin slab
            {
                glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // Would come from layer
                glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.3f, 0.7f); // Yellow for normalization
                
                // Calculate dimensions
                float width = 1.5f;
                float height = 0.2f;
                
                // Create model matrix
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, position);
                model = glm::scale(model, glm::vec3(width, height, 1.0f));
                
                // Use appropriate shader
                m_neuronShader->use();
                m_neuronShader->setUniform("model", model);
                m_neuronShader->setUniform("color", color);
                
                // Render a quad
                m_quadMesh->render();
            }
            break;
            
        case LayerType::OUTPUT:
            // Render as a larger grid
            {
                glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // Would come from layer
                glm::vec4 color = glm::vec4(0.8f, 0.4f, 0.8f, 0.7f); // Purple for output
                
                // Calculate dimensions
                float width = 3.0f;
                float height = 3.0f;
                
                // Create model matrix
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, position);
                model = glm::scale(model, glm::vec3(width, height, 0.1f));
                
                // Use appropriate shader
                m_neuronShader->use();
                m_neuronShader->setUniform("model", model);
                m_neuronShader->setUniform("color", color);
                
                // Render a quad
                m_quadMesh->render();
            }
            break;
            
        case LayerType::ATTENTION:
        case LayerType::FEEDFORWARD:
            // These are handled separately in Layer::render()
            break;
    }
}

void Renderer::renderNeuron(const glm::vec3& position, float size, const glm::vec4& color) {
    if (!m_neuronShader || !m_sphereMesh) return;
    
    m_neuronShader->use();
    
    // Calculate model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(size));
    
    // Set uniforms
    m_neuronShader->setUniform("model", model);
    m_neuronShader->setUniform("color", color);
    
    // Render sphere
    m_sphereMesh->render();
}

void Renderer::renderConnection(const glm::vec3& from, const glm::vec3& to, float strength, const glm::vec4& color) {
    if (!m_connectionShader || !m_cylinderMesh) return;
    
    m_connectionShader->use();
    
    // Calculate direction and length
    glm::vec3 direction = to - from;
    float length = glm::length(direction);
    
    if (length < 0.0001f) return; // Avoid zero-length connections
    
    // Normalize direction
    direction = glm::normalize(direction);
    
    // Calculate rotation from default cylinder orientation (along y-axis)
    glm::vec3 defaultDir(0.0f, 1.0f, 0.0f);
    glm::vec3 rotationAxis = glm::cross(defaultDir, direction);
    
    if (glm::length(rotationAxis) < 0.0001f) {
        // If direction is parallel to defaultDir, use a different rotation axis
        rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    } else {
        rotationAxis = glm::normalize(rotationAxis);
    }
    
    float rotationAngle = acos(glm::dot(defaultDir, direction));
    
    // Calculate model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, from);
    model = glm::rotate(model, rotationAngle, rotationAxis);
    model = glm::scale(model, glm::vec3(0.05f, length, 0.05f)); // Adjust thickness as needed
    
    // Set uniforms
    m_connectionShader->setUniform("model", model);
    m_connectionShader->setUniform("color", color);
    m_connectionShader->setUniform("strength", strength);
    
    // Render cylinder
    m_cylinderMesh->render();
}

void Renderer::renderDataFlow(const glm::vec3& start, const glm::vec3& end, float progress, const glm::vec4& color) {
    m_dataFlowShader->use();
    
    // Calculate position along the path
    glm::vec3 position = start + (end - start) * progress;
    
    // Create model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(0.1f)); // Small size for data particle
    
    // Set uniforms
    m_dataFlowShader->setUniform("model", model);
    m_dataFlowShader->setUniform("color", color);
    m_dataFlowShader->setUniform("progress", progress);
    
    // Render sphere for data particle
    m_sphereMesh->render();
}

void Renderer::renderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color) {
    // Enable blending for text
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    m_textShader->use();
    
    // Set orthographic projection with inverted Y axis to match screen coordinates
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_width), 
                                    static_cast<float>(m_height), 0.0f); // Note inverted Y
    m_textShader->setUniform("projection", projection);
    m_textShader->setUniform("textColor", color);
    
    // Bind VAO and texture
    glBindVertexArray(m_textVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fontTexture);
    
    // Since we don't have a real font, just render each character as a solid rectangle
    float x = position.x;
    for (char c : text) {
        if (c == ' ') {
            // Handle spaces specially
            x += 8.0f * scale;
            continue;
        }
        
        // Simple character size
        float charWidth = 10.0f * scale;
        float charHeight = 16.0f * scale;
        
        // Create a simple quad for each character
        float vertices[6][4] = {
            { x,          position.y,                0.0f, 0.0f },
            { x,          position.y + charHeight,   0.0f, 1.0f },
            { x + charWidth, position.y + charHeight,   1.0f, 1.0f },
            
            { x,          position.y,                0.0f, 0.0f },
            { x + charWidth, position.y + charHeight,   1.0f, 1.0f },
            { x + charWidth, position.y,                1.0f, 0.0f }
        };
        
        // Update buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Move to next character position
        x += charWidth + 2.0f * scale; // Add some spacing between characters
    }
    
    // Clean up
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::renderRect(float x, float y, float width, float height, const glm::vec4& color) {
    // Save current state
    GLboolean depthTest;
    glGetBooleanv(GL_DEPTH_TEST, &depthTest);
    
    // Disable depth testing for UI elements
    glDisable(GL_DEPTH_TEST);
    
    // Use text shader (or create a UI shader if needed)
    m_textShader->use();
    
    // Setup orthographic projection for UI
    glm::mat4 projection = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f);
    m_textShader->setUniform("projection", projection);
    
    // Bind VAO
    glBindVertexArray(m_textVAO);
    
    // Prepare vertices (positions in screen space)
    float vertices[6][4] = {
        { x,         y + height, 0.0f, 0.0f },
        { x,         y,          0.0f, 1.0f },
        { x + width, y,          1.0f, 1.0f },
        
        { x,         y + height, 0.0f, 0.0f },
        { x + width, y,          1.0f, 1.0f },
        { x + width, y + height, 1.0f, 0.0f }
    };
    
    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Set color
    m_textShader->setUniform("textColor", color);
    
    // Set texture (use white texture or create solid color shader)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fontTexture);
    
    // Draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Unbind
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Restore state
    if (depthTest) glEnable(GL_DEPTH_TEST);
}

void Renderer::loadShaders() {
    // Neuron shader
    m_neuronShader = std::make_unique<Shader>();
    if (!m_neuronShader->loadFromSource(
        // Vertex shader
        R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            
            out vec3 Normal;
            out vec3 FragPos;
            
            void main() {
                FragPos = vec3(model * vec4(aPos, 1.0));
                Normal = mat3(transpose(inverse(model))) * aNormal;
                gl_Position = projection * view * vec4(FragPos, 1.0);
            }
        )",
        // Fragment shader
        R"(
            #version 330 core
            in vec3 Normal;
            in vec3 FragPos;
            
            uniform vec4 color;
            
            out vec4 FragColor;
            
            void main() {
                // Simple lighting calculation
                vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
                float diff = max(dot(normalize(Normal), lightDir), 0.0);
                vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
                vec3 ambient = vec3(0.3, 0.3, 0.3);
                
                vec3 result = (ambient + diffuse) * color.rgb;
                FragColor = vec4(result, color.a);
            }
        )"
    )) {
        std::cerr << "Failed to load neuron shader" << std::endl;
    }
    
    // Connection shader
    m_connectionShader = std::make_unique<Shader>();
    if (!m_connectionShader->loadFromSource(
        // Vertex shader (same as neuron shader)
        R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            
            out vec3 Normal;
            out vec3 FragPos;
            
            void main() {
                FragPos = vec3(model * vec4(aPos, 1.0));
                Normal = mat3(transpose(inverse(model))) * aNormal;
                gl_Position = projection * view * vec4(FragPos, 1.0);
            }
        )",
        // Fragment shader
        R"(
            #version 330 core
            in vec3 Normal;
            in vec3 FragPos;
            
            uniform vec4 color;
            
            out vec4 FragColor;
            
            void main() {
                // Simple lighting calculation
                vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
                float diff = max(dot(normalize(Normal), lightDir), 0.0);
                vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
                vec3 ambient = vec3(0.1, 0.1, 0.1);
                
                vec3 result = (ambient + diffuse) * color.rgb;
                FragColor = vec4(result, color.a);
            }
        )"
    )) {
        std::cerr << "Failed to load connection shader" << std::endl;
    }
    
    // Data flow shader
    m_dataFlowShader = std::make_unique<Shader>();
    if (!m_dataFlowShader->loadFromSource(
        // Vertex shader
        R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            uniform float progress;
            
            out float Progress;
            
            void main() {
                gl_Position = projection * view * model * vec4(aPos, 1.0);
                Progress = progress;
            }
        )",
        // Fragment shader
        R"(
            #version 330 core
            in float Progress;
            
            uniform vec4 color;
            
            out vec4 FragColor;
            
            void main() {
                // Pulsating effect based on progress
                float pulse = (sin(Progress * 10.0) + 1.0) * 0.5;
                vec3 glowColor = color.rgb * (0.5 + 0.5 * pulse);
                FragColor = vec4(glowColor, color.a);
            }
        )"
    )) {
        std::cerr << "Failed to load data flow shader" << std::endl;
    }
    
    // Text shader
    m_textShader = std::make_unique<Shader>();
    if (!m_textShader->loadFromSource(
        // Vertex shader
        R"(
            #version 330 core
            layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
            
            uniform mat4 projection;
            
            out vec2 TexCoords;
            
            void main() {
                gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
                TexCoords = vertex.zw;
            }
        )",
        // Fragment shader
        R"(
            #version 330 core
            in vec2 TexCoords;
            
            uniform sampler2D text;
            uniform vec4 textColor;
            
            out vec4 FragColor;
            
            void main() {
                vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
                FragColor = textColor * sampled;
            }
        )"
    )) {
        std::cerr << "Failed to load text shader" << std::endl;
    }
}

void Renderer::createMeshes() {
    // Create sphere mesh
    m_sphereMesh = std::make_unique<Mesh>();
    m_sphereMesh->createSphere(1.0f, 16);
    
    // Create cylinder mesh
    m_cylinderMesh = std::make_unique<Mesh>();
    m_cylinderMesh->createCylinder(1.0f, 1.0f, 16);
    
    // Create quad mesh
    m_quadMesh = std::make_unique<Mesh>();
    m_quadMesh->createQuad(1.0f, 1.0f);
    
    // Create VBO for text rendering
    glGenBuffers(1, &m_textVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    
    // Setup VAO for text rendering
    glGenVertexArrays(1, &m_textVAO);
    glBindVertexArray(m_textVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::loadFonts() {
    // In a real implementation, this would load a font texture
    // For simplicity, create a basic font texture with a single color
    glGenTextures(1, &m_fontTexture);
    glBindTexture(GL_TEXTURE_2D, m_fontTexture);
    
    // Create a simple white texture (single color)
    const int texSize = 64;
    unsigned char data[texSize * texSize];
    for (int i = 0; i < texSize * texSize; i++) {
        data[i] = 255; // White
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texSize, texSize, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::setCamera(Camera* camera) { m_camera = camera; }

} // namespace llmvis 