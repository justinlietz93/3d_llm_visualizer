#include "Model.h"
#include "Layer.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <string>

namespace llmvis {

Model::Model()
    : m_simulationSpeed(1.0f)
    , m_currentStep(0)
    , m_animateDataFlow(false)
    , m_currentInput("")
    , m_embeddingData()
{
}

Model::~Model() {
}

bool Model::loadFromFile(const std::string& modelPath) {
    // For demonstration, we'll create a simple transformer model structure
    // In a real implementation, this would parse a model file
    
    std::cout << "Loading model from: " << modelPath << std::endl;
    
    // Clear existing layers
    m_layers.clear();
    
    // Create a simple transformer model architecture
    // 1. Embedding layer
    m_layers.push_back(std::make_unique<Layer>(LayerType::EMBEDDING, 512));
    
    // 2. Several transformer blocks
    for (int i = 0; i < 4; i++) {
        // Attention layer
        m_layers.push_back(std::make_unique<Layer>(LayerType::ATTENTION, 512));
        
        // Normalization layer
        m_layers.push_back(std::make_unique<Layer>(LayerType::NORMALIZATION, 512));
        
        // Feed-forward layer
        m_layers.push_back(std::make_unique<Layer>(LayerType::FEEDFORWARD, 2048));
        
        // Normalization layer
        m_layers.push_back(std::make_unique<Layer>(LayerType::NORMALIZATION, 512));
    }
    
    // 3. Output layer
    m_layers.push_back(std::make_unique<Layer>(LayerType::OUTPUT, 50000)); // Vocabulary size
    
    // Position layers in 3D space
    float layerSpacing = 1.5f;
    float yOffset = 0.0f;
    
    for (int i = 0; i < m_layers.size(); i++) {
        Layer* layer = m_layers[i].get();
        
        // Calculate layer position
        float x = 0.0f;
        float y = yOffset;
        float z = i * layerSpacing;
        
        // For demonstration, set a simulated position
        layer->setPosition(glm::vec3(x, y, z));
        
        // Adjust Y offset for certain layer types
        if (layer->getType() == LayerType::ATTENTION) {
            yOffset += 0.5f;
        } else if (layer->getType() == LayerType::FEEDFORWARD) {
            yOffset -= 0.5f;
        }
    }
    
    // Create a simple token mapping
    m_tokenToIdMap["[START]"] = 0;
    m_tokenToIdMap["Hello"] = 1;
    m_tokenToIdMap["world"] = 2;
    m_tokenToIdMap["AI"] = 3;
    m_tokenToIdMap["model"] = 4;
    m_tokenToIdMap["visualization"] = 5;
    m_tokenToIdMap["is"] = 6;
    m_tokenToIdMap["cool"] = 7;
    m_tokenToIdMap["!"] = 8;
    m_tokenToIdMap["[END]"] = 9;
    
    return true;
}

void Model::update(float deltaTime) {
    // Update all layers
    for (auto& layer : m_layers) {
        layer->update(deltaTime);
    }
    
    // If there's input being processed, animate data flowing through the network
    if (!m_currentInput.empty() && m_animateDataFlow) {
        // Calculate which layer should be activated based on current step
        float totalSteps = m_layers.size() * 10.0f; // 10 steps per layer
        float currentProgress = fmod(m_currentStep * deltaTime * m_simulationSpeed, totalSteps) / totalSteps;
        
        int activeLayerIndex = static_cast<int>(currentProgress * m_layers.size());
        float layerProgress = fmod(currentProgress * m_layers.size(), 1.0f);
        
        // Set activation level for the active layer
        for (int i = 0; i < m_layers.size(); i++) {
            if (i < activeLayerIndex) {
                m_layers[i]->setActivation(1.0f); // Past layers are fully activated
            } else if (i == activeLayerIndex) {
                m_layers[i]->setActivation(layerProgress); // Current layer is partially activated
            } else {
                m_layers[i]->setActivation(0.0f); // Future layers are not activated yet
            }
        }
        
        m_currentStep++;
    }
}

void Model::render(Renderer* renderer) {
    // Render all layers
    for (auto& layer : m_layers) {
        layer->render(renderer);
    }
}

void Model::processInput(const std::string& input) {
    m_currentInput = input;
    m_currentStep = 0;
    
    // Tokenize input (simplified version)
    std::vector<int> tokens;
    size_t start = 0, end;
    
    while ((end = input.find(' ', start)) != std::string::npos) {
        std::string token = input.substr(start, end - start);
        if (m_tokenToIdMap.find(token) != m_tokenToIdMap.end()) {
            tokens.push_back(m_tokenToIdMap[token]);
        }
        start = end + 1;
    }
    
    // Don't forget the last word
    std::string lastToken = input.substr(start);
    if (m_tokenToIdMap.find(lastToken) != m_tokenToIdMap.end()) {
        tokens.push_back(m_tokenToIdMap[lastToken]);
    }
    
    // Convert tokens to embedding (very simplified)
    m_embeddingData.clear();
    m_embeddingData.resize(512, 0.0f); // Embedding size
    
    // For visualization purposes, just set some random values
    for (int i = 0; i < m_embeddingData.size(); i++) {
        m_embeddingData[i] = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
    }
    
    // Process the embedding through the first layer
    if (!m_layers.empty()) {
        m_layers[0]->processInput(m_embeddingData);
    }
}

std::string Model::getCurrentActivation() {
    return "Not implemented";
}

void Model::setSimulationSpeed(float speed) {
    m_simulationSpeed = speed;
}

void Model::highlightLayer(int layerIndex) {
    if (layerIndex >= 0 && layerIndex < m_layers.size()) {
        // Unhighlight all layers first
        for (auto& layer : m_layers) {
            layer->highlight(false);
        }
        
        // Highlight the selected layer
        m_layers[layerIndex]->highlight(true);
    }
}

void Model::highlightAttentionHead(int layerIndex, int headIndex) {
    if (layerIndex >= 0 && layerIndex < m_layers.size()) {
        Layer* layer = m_layers[layerIndex].get();
        if (layer->getType() == LayerType::ATTENTION) {
            layer->highlightAttentionHead(headIndex);
        }
    }
}

Layer* Model::getLayer(int index) {
    if (index >= 0 && index < m_layers.size()) {
        return m_layers[index].get();
    }
    return nullptr;
}

int Model::getLayerCount() const {
    return m_layers.size();
}

} // namespace llmvis 