#include "AttentionHead.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace llmvis {

AttentionHead::AttentionHead(int id, int dimensions)
    : m_id(id)
    , m_dimensions(dimensions)
    , m_isHighlighted(false)
    , m_position(0.0f)
    , m_visualScale(1.0f)
{
    // Initialize random matrices (for visualization purposes)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-0.5f, 0.5f);
    
    // Query matrix
    m_queryMatrix.resize(dimensions);
    for (auto& row : m_queryMatrix) {
        row.resize(dimensions);
        for (auto& val : row) {
            val = dis(gen);
        }
    }
    
    // Key matrix
    m_keyMatrix.resize(dimensions);
    for (auto& row : m_keyMatrix) {
        row.resize(dimensions);
        for (auto& val : row) {
            val = dis(gen);
        }
    }
    
    // Value matrix
    m_valueMatrix.resize(dimensions);
    for (auto& row : m_valueMatrix) {
        row.resize(dimensions);
        for (auto& val : row) {
            val = dis(gen);
        }
    }
    
    // Initialize output and attention weights
    m_output.resize(dimensions, 0.0f);
    m_attentionWeights.resize(1); // Will be resized during computation
}

AttentionHead::~AttentionHead() {
}

void AttentionHead::update(float deltaTime) {
    // Animation logic could go here
}

void AttentionHead::computeAttention(const std::vector<float>& queryInput, 
                                    const std::vector<float>& keyInput,
                                    const std::vector<float>& valueInput) {
    // Simplified attention mechanism for visualization
    // In a real implementation, this would be a proper self-attention calculation
    
    int sequenceLength = queryInput.size() / m_dimensions;
    
    // Resize attention weights matrix
    m_attentionWeights.resize(sequenceLength);
    for (auto& row : m_attentionWeights) {
        row.resize(sequenceLength, 0.0f);
    }
    
    // Compute query, key, value projections (simplified)
    std::vector<float> queryProjection(m_dimensions, 0.0f);
    std::vector<float> keyProjection(m_dimensions, 0.0f);
    std::vector<float> valueProjection(m_dimensions, 0.0f);
    
    // Just for visualization, use random projections
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    // Fill attention weights with random values (for visualization)
    float sum = 0.0f;
    for (int i = 0; i < sequenceLength; i++) {
        for (int j = 0; j < sequenceLength; j++) {
            m_attentionWeights[i][j] = dis(gen);
            sum += m_attentionWeights[i][j];
        }
    }
    
    // Normalize attention weights (softmax-like)
    for (int i = 0; i < sequenceLength; i++) {
        for (int j = 0; j < sequenceLength; j++) {
            m_attentionWeights[i][j] /= sum;
        }
    }
    
    // Create output based on attention (simplified for visualization)
    for (int i = 0; i < m_dimensions; i++) {
        m_output[i] = dis(gen) * 2.0f - 1.0f;
    }
}

const std::vector<float>& AttentionHead::getOutput() const {
    return m_output;
}

const std::vector<std::vector<float>>& AttentionHead::getAttentionWeights() const {
    return m_attentionWeights;
}

void AttentionHead::setHighlighted(bool isHighlighted) {
    m_isHighlighted = isHighlighted;
}

bool AttentionHead::isHighlighted() const {
    return m_isHighlighted;
}

const glm::vec3& AttentionHead::getPosition() const {
    return m_position;
}

void AttentionHead::setPosition(const glm::vec3& position) {
    m_position = position;
}

} // namespace llmvis 