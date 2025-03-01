#include "SimulationController.h"
#include "Model.h"
#include <iostream>

namespace llmvis {

SimulationController::SimulationController(Model* model)
    : m_model(model)
    , m_speed(1.0f)
    , m_isPaused(false)
    , m_currentStep(0)
{
    setupDefaultExperiments();
}

SimulationController::~SimulationController() {
}

void SimulationController::update(float deltaTime) {
    // Update simulation logic if not paused
    if (!m_isPaused) {
        // Nothing to do here - model updates itself
    }
}

void SimulationController::setSpeed(float speed) {
    m_speed = speed;
}

void SimulationController::pause() {
    m_isPaused = true;
}

void SimulationController::resume() {
    m_isPaused = false;
}

void SimulationController::stepForward() {
    // Check if already at max step
    if (m_currentStep >= 100) { // Add reasonable upper limit
        return;
    }
    
    m_currentStep++;
    std::cout << "Advancing to step " << m_currentStep << std::endl;
    
    // Apply the step to the model (if necessary)
    // Be careful with null pointer checks
    if (m_model) {
        // Safe model operations here
    }
}

void SimulationController::stepBackward() {
    // Check if already at first step
    if (m_currentStep <= 0) {
        return;
    }
    
    m_currentStep--;
    std::cout << "Going back to step " << m_currentStep << std::endl;
    
    // Apply the step to the model (if necessary)
    if (m_model) {
        // Safe model operations here
    }
}

void SimulationController::runExperiment(ExperimentType type) {
    auto it = m_experiments.find(type);
    if (it != m_experiments.end()) {
        std::cout << "Running experiment: " << static_cast<int>(type) << std::endl;
        it->second();
    } else {
        std::cout << "Experiment not found: " << static_cast<int>(type) << std::endl;
    }
}

void SimulationController::injectPrompt(const std::string& prompt) {
    std::cout << "Injecting prompt: " << prompt << std::endl;
    
    // Pass the prompt to the model for processing
    m_model->processInput(prompt);
}

void SimulationController::saveCurrentState(const std::string& fileName) {
    std::cout << "Saving state to: " << fileName << std::endl;
    
    // In a real implementation, this would serialize the model state
    // For now, just a placeholder
}

void SimulationController::loadState(const std::string& fileName) {
    std::cout << "Loading state from: " << fileName << std::endl;
    
    // In a real implementation, this would deserialize the model state
    // For now, just a placeholder
}

void SimulationController::registerExperiment(ExperimentType type, std::function<void()> experimentFunc) {
    m_experiments[type] = experimentFunc;
}

void SimulationController::setupDefaultExperiments() {
    // Register default experiments
    
    // 1. Change attention weights experiment
    registerExperiment(ExperimentType::CHANGE_ATTENTION_WEIGHTS, [this]() {
        std::cout << "Changing attention weights..." << std::endl;
        
        // Select a random attention layer
        int layerCount = m_model->getLayerCount();
        for (int i = 0; i < layerCount; i++) {
            Layer* layer = m_model->getLayer(i);
            if (layer && layer->getType() == LayerType::ATTENTION) {
                // Modify weights in a random attention head
                int headCount = layer->getAttentionHeadCount();
                if (headCount > 0) {
                    int headIndex = rand() % headCount;
                    std::cout << "Modifying attention head " << headIndex << " in layer " << i << std::endl;
                    
                    // Highlight the modified head
                    m_model->highlightAttentionHead(i, headIndex);
                    
                    // In a real implementation, we would modify the weights here
                    break;
                }
            }
        }
    });
    
    // 2. Modify layer sizes experiment
    registerExperiment(ExperimentType::MODIFY_LAYER_SIZES, [this]() {
        std::cout << "Modifying layer sizes..." << std::endl;
        
        // In a real implementation, this would modify the layer sizes
        // For now, just highlight a random layer
        int layerCount = m_model->getLayerCount();
        if (layerCount > 0) {
            int layerIndex = rand() % layerCount;
            m_model->highlightLayer(layerIndex);
        }
    });
    
    // 3. Alter activation functions experiment
    registerExperiment(ExperimentType::ALTER_ACTIVATION_FUNCTIONS, [this]() {
        std::cout << "Altering activation functions..." << std::endl;
        
        // In a real implementation, this would modify activation functions
        // For now, just highlight a random feedforward layer
        int layerCount = m_model->getLayerCount();
        for (int i = 0; i < layerCount; i++) {
            Layer* layer = m_model->getLayer(i);
            if (layer && layer->getType() == LayerType::FEEDFORWARD) {
                std::cout << "Modifying activation function in layer " << i << std::endl;
                m_model->highlightLayer(i);
                break;
            }
        }
    });
    
    // 4. Inject knowledge experiment
    registerExperiment(ExperimentType::INJECT_KNOWLEDGE, [this]() {
        std::cout << "Injecting knowledge..." << std::endl;
        
        // This would modify the model's internal knowledge
        // For now, just inject a test prompt
        injectPrompt("AI model visualization is cool!");
    });
    
    // 5. Test robustness experiment
    registerExperiment(ExperimentType::TEST_ROBUSTNESS, [this]() {
        std::cout << "Testing model robustness..." << std::endl;
        
        // This would test the model with various inputs
        // For now, just a placeholder
        injectPrompt("This is a test of model robustness!");
    });
}

} // namespace llmvis 