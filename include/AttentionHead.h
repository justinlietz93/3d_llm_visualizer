#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace llmvis {

class AttentionHead {
public:
    AttentionHead(int id, int dimensions);
    ~AttentionHead();
    
    void update(float deltaTime);
    void computeAttention(const std::vector<float>& queryInput, 
                          const std::vector<float>& keyInput,
                          const std::vector<float>& valueInput);
    
    const std::vector<float>& getOutput() const;
    const std::vector<std::vector<float>>& getAttentionWeights() const;
    
    void setHighlighted(bool isHighlighted);
    bool isHighlighted() const;
    
    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);
    
private:
    int m_id;
    int m_dimensions;
    bool m_isHighlighted;
    
    std::vector<float> m_output;
    std::vector<std::vector<float>> m_attentionWeights;
    
    // For visualization
    glm::vec3 m_position;
    float m_visualScale;
    
    // Attention matrices
    std::vector<std::vector<float>> m_queryMatrix;
    std::vector<std::vector<float>> m_keyMatrix;
    std::vector<std::vector<float>> m_valueMatrix;
};

} // namespace llmvis 