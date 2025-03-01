#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Neuron.h"
#include "AttentionHead.h"

namespace llmvis {

enum class LayerType {
    EMBEDDING,
    ATTENTION,
    FEEDFORWARD,
    NORMALIZATION,
    OUTPUT
};

class Layer {
public:
    Layer(LayerType type, int size);
    ~Layer();
    
    void update(float deltaTime);
    void render(class Renderer* renderer);
    
    void processInput(const std::vector<float>& input);
    std::vector<float> getOutput() const;
    
    void setActivation(float progress);
    void highlight(bool isHighlighted);
    
    LayerType getType() const { return m_type; }
    int getSize() const { return m_size; }
    
    // For attention layers
    void highlightAttentionHead(int headIndex);
    AttentionHead* getAttentionHead(int index);
    int getAttentionHeadCount() const;
    
    // Add missing position functions
    const glm::vec3& getPosition() const { return m_position; }
    void setPosition(const glm::vec3& position);
    
private:
    LayerType m_type;
    int m_size;
    bool m_isHighlighted;
    float m_activationProgress;
    
    std::vector<float> m_inputValues;
    std::vector<float> m_outputValues;
    
    // For attention layers
    std::vector<std::unique_ptr<AttentionHead>> m_attentionHeads;
    
    // For visualization
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_color;
};

} // namespace llmvis 