#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Renderer.h"

namespace llmvis {

// Forward declarations
class Model;
class Layer;
class Camera;
class SimulationController;

class LLMVisualization {
public:
    LLMVisualization();
    ~LLMVisualization();
    
    bool initialize(int width, int height);
    void update(float deltaTime);
    void render();
    
    void loadModel(const std::string& modelPath);
    void setSimulationSpeed(float speed);
    void processInput();
    
    // Interactive methods
    void selectComponent(const glm::vec3& rayOrigin, const glm::vec3& rayDirection);
    void modifySelectedComponent(const std::string& property, float value);
    void runExperiment(const std::string& experimentType);
    
    // Accessor for renderer
    Renderer* getRenderer() const { return m_renderer.get(); }
    
    // Add these members
    enum class MenuOption {
        RESUME,
        SETTINGS,
        ABOUT,
        QUIT
    };
    
private:
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Model> m_model;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<SimulationController> m_simulationController;
    
    int m_width;
    int m_height;
    float m_simulationSpeed;
    bool m_isPaused;
    
    // Add these members
    bool m_showPauseMenu;
    int m_selectedMenuOption;
    
    // Add these methods
    void renderPauseMenu();
    void handleMenuInput();
    bool processMenuOption(MenuOption option);
};

} // namespace llmvis 