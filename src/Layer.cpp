#include "Layer.h"
#include "Renderer.h"
#include <algorithm>
#include <iostream>

namespace llmvis {

Layer::Layer(LayerType type, int size)
    : m_type(type)
    , m_size(size)
    , m_isHighlighted(false)
    , m_activationProgress(0.0f)
    , m_position(0.0f)
    , m_scale(1.0f)
{
    // Initialize input and output values
    m_inputValues.resize(size, 0.0f);
    m_outputValues.resize(size, 0.0f);
    
    // Initialize color based on layer type
    switch (type) {
        case LayerType::EMBEDDING:
            m_color = glm::vec3(0.2f, 0.6f, 0.8f); // Blue
            break;
        case LayerType::ATTENTION:
            m_color = glm::vec3(0.8f, 0.3f, 0.3f); // Red
            // Create attention heads
            for (int i = 0; i < 8; i++) { // Assuming 8 attention heads
                m_attentionHeads.push_back(std::make_unique<AttentionHead>(i, size / 8));
            }
            break;
        case LayerType::FEEDFORWARD:
            m_color = glm::vec3(0.3f, 0.8f, 0.3f); // Green
            break;
        case LayerType::NORMALIZATION:
            m_color = glm::vec3(0.8f, 0.8f, 0.3f); // Yellow
            break;
        case LayerType::OUTPUT:
            m_color = glm::vec3(0.8f, 0.4f, 0.8f); // Purple
            break;
    }
}

Layer::~Layer() {
}

void Layer::update(float deltaTime) {
    // Update attention heads if this is an attention layer
    if (m_type == LayerType::ATTENTION) {
        for (auto& head : m_attentionHeads) {
            head->update(deltaTime);
        }
    }
}

void Layer::render(Renderer* renderer) {
    // Determine visualization style based on layer type
    glm::vec4 color = glm::vec4(m_color, 1.0f);
    
    // If highlighted, make it brighter
    if (m_isHighlighted) {
        color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    // Apply activation progress to color alpha
    color.a = 0.3f + m_activationProgress * 0.7f;
    
    // Scale based on layer size
    float sizeScale = std::log10(m_size) * 0.2f;
    
    // Render based on layer type
    switch (m_type) {
        case LayerType::EMBEDDING:
            // Render as a flat rectangular grid
            renderer->renderLayer(this);
            break;
        case LayerType::ATTENTION:
            // Render attention heads
            for (int i = 0; i < m_attentionHeads.size(); i++) {
                auto& head = m_attentionHeads[i];
                
                // Position the head in a circular arrangement
                float angle = (static_cast<float>(i) / m_attentionHeads.size()) * 2.0f * 3.14159f;
                float radius = 1.0f;
                glm::vec3 headPos = m_position + glm::vec3(cos(angle) * radius, sin(angle) * radius, 0.0f);
                
                head->setPosition(headPos);
                
                // Render the head
                float headSize = 0.2f;
                glm::vec4 headColor = head->isHighlighted() ? glm::vec4(1.0f) : glm::vec4(color);
                renderer->renderNeuron(headPos, headSize, headColor);
                
                // Render connections between heads
                if (i > 0) {
                    glm::vec3 prevHeadPos = m_attentionHeads[i-1]->getPosition();
                    renderer->renderConnection(prevHeadPos, headPos, 0.5f, color);
                }
            }
            break;
        case LayerType::FEEDFORWARD:
            // Render as a collection of neurons
            // For simplicity, just render a representation rather than each neuron
            {
                int neuronsPerRow = static_cast<int>(sqrt(std::min(100, m_size)));
                float spacing = 0.2f;
                
                for (int i = 0; i < std::min(100, m_size); i++) {
                    int row = i / neuronsPerRow;
                    int col = i % neuronsPerRow;
                    
                    glm::vec3 neuronPos = m_position + glm::vec3(
                        (col - neuronsPerRow / 2) * spacing,
                        (row - neuronsPerRow / 2) * spacing,
                        0.0f
                    );
                    
                    renderer->renderNeuron(neuronPos, 0.05f, color);
                }
            }
            break;
        case LayerType::NORMALIZATION:
            // Render as a thin rectangular slab
            renderer->renderLayer(this);
            break;
        case LayerType::OUTPUT:
            // Render as a large output grid
            renderer->renderLayer(this);
            break;
    }
}

void Layer::processInput(const std::vector<float>& input) {
    // Store input values
    m_inputValues = input;
    
    // Process based on layer type
    switch (m_type) {
        case LayerType::EMBEDDING: {
            // Simple pass-through for embedding (in real implementation, would convert token to embedding)
            m_outputValues = input;
            break;
        }
        
        case LayerType::ATTENTION: {
            // In a real implementation, this would compute attention
            m_outputValues.resize(m_size, 0.0f);
            
            // For visualization, just use dummy values for now
            for (size_t i = 0; i < m_outputValues.size(); ++i) {
                m_outputValues[i] = sin(i * 0.1f) * 0.5f + 0.5f;
            }
            break;
        }
        
        case LayerType::FEEDFORWARD: {
            // Simple activation function (ReLU)
            m_outputValues.resize(input.size());
            for (size_t i = 0; i < input.size(); ++i) {
                m_outputValues[i] = std::max(0.0f, input[i]);
            }
            break;
        }
        
        case LayerType::NORMALIZATION: {
            // Layer normalization
            float sum = 0.0f;
            for (float val : input) {
                sum += val;
            }
            float mean = sum / input.size();
            
            // Apply normalization
            m_outputValues.resize(input.size());
            for (size_t i = 0; i < input.size(); ++i) {
                m_outputValues[i] = input[i] - mean;
            }
            break;
        }
        
        case LayerType::OUTPUT: {
            // Simple softmax-like function
            m_outputValues.resize(input.size());
            
            // Find max for numerical stability
            float maxVal = *std::max_element(input.begin(), input.end());
            
            // Apply exp and sum
            float sum = 0.0f;
            for (size_t i = 0; i < input.size(); ++i) {
                m_outputValues[i] = exp(input[i] - maxVal);
                sum += m_outputValues[i];
            }
            
            // Normalize
            for (size_t i = 0; i < m_outputValues.size(); ++i) {
                m_outputValues[i] /= sum;
            }
            break;
        }
    }
}

std::vector<float> Layer::getOutput() const {
    return m_outputValues;
}

void Layer::setActivation(float progress) {
    m_activationProgress = progress;
}

void Layer::highlight(bool isHighlighted) {
    m_isHighlighted = isHighlighted;
}

void Layer::highlightAttentionHead(int headIndex) {
    if (m_type == LayerType::ATTENTION && headIndex >= 0 && headIndex < m_attentionHeads.size()) {
        // First unhighlight all heads
        for (auto& head : m_attentionHeads) {
            head->setHighlighted(false);
        }
        
        // Highlight selected head
        m_attentionHeads[headIndex]->setHighlighted(true);
    }
}

AttentionHead* Layer::getAttentionHead(int index) {
    if (m_type == LayerType::ATTENTION && index >= 0 && index < m_attentionHeads.size()) {
        return m_attentionHeads[index].get();
    }
    return nullptr;
}

int Layer::getAttentionHeadCount() const {
    if (m_type == LayerType::ATTENTION) {
        return m_attentionHeads.size();
    }
    return 0;
}

void Layer::setPosition(const glm::vec3& position) {
    m_position = position;
}

} // namespace llmvis 