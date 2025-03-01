cmake_minimum_required(VERSION 3.10)
project(LLMVisualization LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/external)
include_directories(${PROJECT_SOURCE_DIR}/external/glad/include)
include_directories(${GLM_INCLUDE_DIRS})

# Add source files explicitly rather than using glob
set(SOURCES
    src/main.cpp
    src/LLMVisualization.cpp
    src/Camera.cpp
    src/Layer.cpp
    src/Model.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/Mesh.cpp
    src/Neuron.cpp
    src/AttentionHead.cpp
    src/SimulationController.cpp
    external/glad/src/glad.c
)

# Add executable
add_executable(llm_visualizer ${SOURCES})

# Link libraries
target_link_libraries(llm_visualizer
    OpenGL::GL
    glfw
    ${CMAKE_DL_LIBS}
)

# Copy resources if they exist
if(EXISTS "${PROJECT_SOURCE_DIR}/resources")
    file(COPY ${PROJECT_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
endif() 