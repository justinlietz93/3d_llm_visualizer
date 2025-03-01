#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "Layer.h"
#include "Common.h"

namespace llmvis {

class Model {
public:
    Model();
    ~Model();
    
    bool initialize();
    void update(float deltaTime);
    void render(class Renderer* renderer);
    bool loadFromFile(const std::string& filePath);
    
    void processInput(const std::string& input);
    void highlightLayer(int layerIndex);
    void highlightAttentionHead(int layerIndex, int headIndex);
    
    void setSimulationSpeed(float speed);
    float getSimulationSpeed() const;
    
    Layer* getLayer(int index);
    int getLayerCount() const;
    
    std::string getCurrentActivation();
    
private:
    std::vector<std::unique_ptr<Layer>> m_layers;
    std::string m_currentInput;
    std::vector<float> m_embeddingData;
    std::unordered_map<std::string, int> m_tokenToIdMap;
    
    float m_simulationSpeed;
    int m_currentStep;
    bool m_animateDataFlow;
    
    // Internal methods
    void setupDefaultModel();
    void connectLayers();
};

} // namespace llmvis 