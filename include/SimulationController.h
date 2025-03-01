#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "Common.h"

namespace llmvis {

class Model;

class SimulationController {
public:
    SimulationController(Model* model);
    ~SimulationController();
    
    void update(float deltaTime);
    void setSpeed(float speed);
    void pause();
    void resume();
    void stepForward();
    void stepBackward();
    
    void runExperiment(ExperimentType type);
    void injectPrompt(const std::string& prompt);
    void saveCurrentState(const std::string& fileName);
    void loadState(const std::string& fileName);
    
    void registerExperiment(ExperimentType type, std::function<void()> experimentFunc);
    
private:
    Model* m_model;
    float m_speed;
    bool m_isPaused;
    int m_currentStep;
    
    std::unordered_map<ExperimentType, std::function<void()>> m_experiments;
    
    void setupDefaultExperiments();
};

} // namespace llmvis 