#include "LLMVisualization.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>  // Add this for sleep
#include <signal.h>

// Global flag for clean shutdown
volatile sig_atomic_t exitSignal = 0;

// Signal handler
void signalHandler(int signal) {
    exitSignal = signal;
    std::cout << "Received signal " << signal << ", initiating clean shutdown..." << std::endl;
}

int main(int argc, char** argv) {
    // Register signal handlers for clean termination
    signal(SIGINT, signalHandler);  // Ctrl+C
    signal(SIGTERM, signalHandler); // Termination request
    
    // Initialize GLFW first to ensure proper setup
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }
    
    // Use a scope to ensure all objects are destroyed before glfwTerminate()
    {
        llmvis::LLMVisualization visualization;
        
        // Try to initialize the visualization
        if (!visualization.initialize(1280, 720)) {
            std::cerr << "Failed to initialize LLM visualization system!" << std::endl;
            glfwTerminate();
            return -1;
        }
        
        // Load a default model if available
        std::string modelPath = "models/tiny_llm.bin";
        if (argc > 1) {
            modelPath = argv[1];
        }
        
        visualization.loadModel(modelPath);
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        // Main loop with error handling
        try {
            while (!glfwWindowShouldClose(visualization.getRenderer()->getWindow()) && !exitSignal) {
                // Calculate delta time
                auto currentTime = std::chrono::high_resolution_clock::now();
                float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
                lastTime = currentTime;
                
                // Process input
                visualization.processInput();
                
                // Update simulation
                visualization.update(deltaTime);
                
                // Render frame
                visualization.render();
                
                // Poll events (critical for input handling)
                glfwPollEvents();
                
                // Small delay to avoid running too fast
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
                
                // Check for exit signal
                if (exitSignal) {
                    glfwSetWindowShouldClose(visualization.getRenderer()->getWindow(), true);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        }
        
        // visualization will be destroyed automatically here
        std::cout << "Application closing - cleaning up resources..." << std::endl;
    }
    
    // Clean up GLFW after all objects using it are destroyed
    glfwTerminate();
    std::cout << "GLFW terminated successfully" << std::endl;
    
    return 0;
} 