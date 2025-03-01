#include "LLMVisualization.h"
#include "Renderer.h"
#include "Model.h"
#include "Camera.h"
#include "SimulationController.h"
#include <iostream>
#include <GLFW/glfw3.h>

namespace llmvis {

LLMVisualization::LLMVisualization() 
    : m_width(0)
    , m_height(0)
    , m_simulationSpeed(1.0f)
    , m_isPaused(false)
    , m_showPauseMenu(false)
    , m_selectedMenuOption(0)
{
}

LLMVisualization::~LLMVisualization() {
    // Make sure to reset the static camera pointer 
    // This prevents callbacks trying to access it after it's destroyed
    static Camera* s_cameraInstance = nullptr;
    
    // Reset cursor mode before destroying window
    if (m_renderer && m_renderer->getWindow()) {
        glfwSetInputMode(m_renderer->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
    // Important: destroy in the correct order
    // First clear the simulation controller (which might reference the model)
    m_simulationController.reset();
    
    // Then clear the camera
    m_camera.reset();
    
    // Clear the model
    m_model.reset();
    
    // Renderer must be destroyed last, as it holds the OpenGL context
    m_renderer.reset();
    
    std::cout << "LLMVisualization cleaned up successfully" << std::endl;
}

bool LLMVisualization::initialize(int width, int height) {
    m_width = width;
    m_height = height;
    
    // Initialize renderer
    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->initialize(width, height)) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false;
    }
    
    // Initialize camera
    m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 5.0f));
    
    // Initialize model
    m_model = std::make_unique<Model>();
    
    // Initialize simulation controller
    m_simulationController = std::make_unique<SimulationController>(m_model.get());
    
    static Camera* s_cameraInstance = nullptr;
    s_cameraInstance = m_camera.get();

    // Set up mouse callback
    glfwSetCursorPosCallback(m_renderer->getWindow(), [](GLFWwindow* window, double xpos, double ypos) {
        static float lastX = 640.0f;
        static float lastY = 360.0f;
        static bool firstMouse = true;
        
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
        
        lastX = xpos;
        lastY = ypos;
        
        if (s_cameraInstance)
            s_cameraInstance->processMouseMovement(xoffset, yoffset, true);
    });
    
    // Enable mouse capture
    glfwSetInputMode(m_renderer->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the camera for rendering
    m_renderer->setCamera(m_camera.get());

    return true;
}

void LLMVisualization::update(float deltaTime) {
    // Update camera
    m_camera->update(deltaTime);
    
    // Update simulation controller
    m_simulationController->update(deltaTime);
    
    // Update model if not paused
    if (!m_isPaused) {
        m_model->update(deltaTime * m_simulationSpeed);
    }
}

void LLMVisualization::render() {
    // Begin frame
    m_renderer->beginFrame();
    
    // Render model
    m_model->render(m_renderer.get());
    
    // Render pause menu if active
    if (m_showPauseMenu) {
        renderPauseMenu();
    }
    
    // End frame
    m_renderer->endFrame();
}

void LLMVisualization::loadModel(const std::string& modelPath) {
    if (!m_model->loadFromFile(modelPath)) {
        std::cerr << "Failed to load model from " << modelPath << std::endl;
    }
}

void LLMVisualization::setSimulationSpeed(float speed) {
    m_simulationSpeed = speed;
    m_model->setSimulationSpeed(speed);
}

void LLMVisualization::processInput() {
    if (!m_renderer || !m_camera) return;
    
    GLFWwindow* window = m_renderer->getWindow();
    
    // Check for ESC to toggle pause menu
    static bool escPressed = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!escPressed) {
            m_showPauseMenu = !m_showPauseMenu;
            
            // Toggle cursor mode
            if (m_showPauseMenu) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            
            escPressed = true;
        }
    } else {
        escPressed = false;
    }
    
    // If pause menu is active, handle menu input instead
    if (m_showPauseMenu) {
        handleMenuInput();
        return;
    }
    
    // Process keyboard input for camera movement
    float deltaTime = 0.05f; // Use a constant delta time for smooth movement
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera->processKeyboard(GLFW_KEY_W, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera->processKeyboard(GLFW_KEY_S, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera->processKeyboard(GLFW_KEY_A, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera->processKeyboard(GLFW_KEY_D, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_camera->processKeyboard(GLFW_KEY_Q, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_camera->processKeyboard(GLFW_KEY_E, deltaTime);
    
    // Toggle pause with space
    static bool spacePressed = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!spacePressed) {
            m_isPaused = !m_isPaused;
            spacePressed = true;
            
            if (m_isPaused) {
                m_simulationController->pause();
            } else {
                m_simulationController->resume();
            }
        }
    } else {
        spacePressed = false;
    }
    
    // Step forward with right arrow
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        m_simulationController->stepForward();
    }
    
    // Step backward with left arrow
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        m_simulationController->stepBackward();
    }
    
    // Speed up with + key
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        setSimulationSpeed(m_simulationSpeed * 1.1f);
    }
    
    // Slow down with - key
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        setSimulationSpeed(m_simulationSpeed * 0.9f);
    }
}

void LLMVisualization::selectComponent(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) {
    // Implementation of ray casting to select components will go here
    // For simplicity, placeholder only
}

void LLMVisualization::modifySelectedComponent(const std::string& property, float value) {
    // Implementation to modify selected component
    // For simplicity, placeholder only
}

void LLMVisualization::runExperiment(const std::string& experimentType) {
    // Implementation to run an experiment
    // For simplicity, placeholder only
    if (experimentType == "CHANGE_ATTENTION_WEIGHTS") {
        m_simulationController->runExperiment(ExperimentType::CHANGE_ATTENTION_WEIGHTS);
    } else if (experimentType == "MODIFY_LAYER_SIZES") {
        m_simulationController->runExperiment(ExperimentType::MODIFY_LAYER_SIZES);
    } else if (experimentType == "ALTER_ACTIVATION_FUNCTIONS") {
        m_simulationController->runExperiment(ExperimentType::ALTER_ACTIVATION_FUNCTIONS);
    } else if (experimentType == "INJECT_KNOWLEDGE") {
        m_simulationController->runExperiment(ExperimentType::INJECT_KNOWLEDGE);
    } else if (experimentType == "TEST_ROBUSTNESS") {
        m_simulationController->runExperiment(ExperimentType::TEST_ROBUSTNESS);
    }
}

void LLMVisualization::renderPauseMenu() {
    if (!m_showPauseMenu) return;
    
    // Enable blending for transparent UI
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable depth test for UI elements
    glDisable(GL_DEPTH_TEST);
    
    // Draw dark semi-transparent background
    m_renderer->renderRect(m_width/2 - 200, m_height/2 - 200, 400, 400, 
                          glm::vec4(0.1f, 0.1f, 0.2f, 0.9f));
    
    // Draw title
    m_renderer->renderText("PAUSE MENU", 
                          glm::vec2(m_width/2 - 80, m_height/2 - 150), 
                          2.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // Menu options
    const std::vector<std::string> options = {
        "Resume", "Settings", "About", "Quit"
    };
    
    // Render each option
    float yStart = m_height/2 - 80;
    float ySpacing = 40;
    
    for (size_t i = 0; i < options.size(); ++i) {
        float y = yStart + i * ySpacing;
        
        // Highlight selected option
        glm::vec4 color = (i == m_selectedMenuOption) ? 
            glm::vec4(1.0f, 0.8f, 0.2f, 1.0f) : // Yellow for selected
            glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);  // White for others
            
        m_renderer->renderText(options[i], 
                              glm::vec2(m_width/2 - 50, y), 
                              1.5f, color);
    }
    
    // Restore OpenGL state
    glEnable(GL_DEPTH_TEST);
}

void LLMVisualization::handleMenuInput() {
    GLFWwindow* window = m_renderer->getWindow();
    
    // Up/down to navigate menu
    static bool upPressed = false;
    static bool downPressed = false;
    static bool enterPressed = false;
    
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (!upPressed) {
            m_selectedMenuOption = (m_selectedMenuOption > 0) ? m_selectedMenuOption - 1 : 3;
            upPressed = true;
        }
    } else {
        upPressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!downPressed) {
            m_selectedMenuOption = (m_selectedMenuOption < 3) ? m_selectedMenuOption + 1 : 0;
            downPressed = true;
        }
    } else {
        downPressed = false;
    }
    
    // Enter to select option
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!enterPressed) {
            processMenuOption(static_cast<MenuOption>(m_selectedMenuOption));
            enterPressed = true;
        }
    } else {
        enterPressed = false;
    }
    
    // Check for mouse clicks on menu items
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    // Also handle mouse clicks here if needed
}

bool LLMVisualization::processMenuOption(MenuOption option) {
    switch (option) {
        case MenuOption::RESUME:
            m_showPauseMenu = false;
            glfwSetInputMode(m_renderer->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
            
        case MenuOption::SETTINGS:
            // TODO: Add settings functionality
            break;
            
        case MenuOption::ABOUT:
            // TODO: Show about information
            break;
            
        case MenuOption::QUIT:
            glfwSetWindowShouldClose(m_renderer->getWindow(), true);
            return true;
    }
    
    return false;
}

} // namespace llmvis 