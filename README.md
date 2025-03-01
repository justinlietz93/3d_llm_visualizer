# OpenGL Graphics Application

A modern C++ application that leverages OpenGL for hardware-accelerated graphics rendering.

## Overview

This project implements a graphics rendering application using modern OpenGL. It provides a layer-based architecture for organizing rendering components and demonstrates various graphics techniques.

## Requirements

- C++ compiler with C++11 support or higher
- CMake 3.10 or higher
- OpenGL 3.3+ compatible graphics hardware and drivers

## Dependencies

- GLAD - OpenGL Loading Library (included in external/glad)
- OpenGL Mathematics (GLM) library for vector/matrix operations
- GLFW for window creation and input handling

## Building the Project

### Linux/macOS

mkdir build
cd build
cmake ..
make

### Windows

mkdir build
cd build
cmake ..

Then open the generated solution file in Visual Studio and build.

## Project Structure

- src/ - Source code files
  - main.cpp - Application entry point and main loop
  - Layer.cpp - Implementation of the rendering layer system for organizing rendering components
- external/ - Third-party libraries
  - glad/ - OpenGL Loading Library (handles loading OpenGL function pointers)
    - include/glad/glad.h - Core OpenGL header
    - include/KHR/khrplatform.h - Platform definitions

## Architecture

The application uses a layer-based architecture to organize different rendering components:

- Each Layer represents a distinct rendering stage or UI component
- Layers can be stacked and rendered in sequence
- The Layer system allows for modular design and separation of concerns

## Usage

After building, run the executable from the build directory:

./OpenGLApp

### Basic Controls

- ESC - Exit application
- F11 - Toggle fullscreen
- WASD - Move camera
- Mouse - Look around

## Features

- Modern OpenGL rendering pipeline
- Layer-based architecture for organizing render components
- Hardware-accelerated graphics rendering
- Shader-based rendering
- Support for textures and materials

## Development

To add a new rendering feature:
1. Create a new Layer subclass
2. Implement the required render/update methods
3. Add the layer to the application's layer stack

## Troubleshooting

- If you encounter "OpenGL function not loaded" errors, check that your GPU supports OpenGL 3.3+
- For build errors, ensure all dependencies are correctly installed

## License

This project is licensed under the MIT License - see the LICENSE file for details.
