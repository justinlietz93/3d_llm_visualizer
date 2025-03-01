#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Print banner
echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}  LLM Visualization Builder     ${NC}"
echo -e "${GREEN}================================${NC}"

# Parse arguments
CLEAN=false
MAKE_ARGS="-j4"  # Default to using 4 cores

for arg in "$@"; do
  case $arg in
    clean)
      CLEAN=true
      ;;
    -j*)
      MAKE_ARGS=$arg
      ;;
    *)
      echo -e "${YELLOW}Unknown argument: $arg${NC}"
      ;;
  esac
done

# Current directory
ROOT_DIR=$(pwd)
BUILD_DIR="$ROOT_DIR/build"

# Clean if requested
if [ "$CLEAN" = true ]; then
  echo -e "${YELLOW}Cleaning build directory...${NC}"
  rm -rf "$BUILD_DIR"
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
  echo -e "${GREEN}Creating build directory...${NC}"
  mkdir -p "$BUILD_DIR"
  if [ $? -ne 0 ]; then
    echo -e "${RED}Failed to create build directory!${NC}"
    exit 1
  fi
fi

# Enter build directory
cd "$BUILD_DIR" || { echo -e "${RED}Failed to enter build directory!${NC}"; exit 1; }

# Run CMake
echo -e "${GREEN}Running CMake...${NC}"
cmake ..
if [ $? -ne 0 ]; then
  echo -e "${RED}CMake configuration failed!${NC}"
  exit 1
fi

# Build the project
echo -e "${GREEN}Building project with$MAKE_ARGS...${NC}"
make $MAKE_ARGS
if [ $? -ne 0 ]; then
  echo -e "${RED}Build failed!${NC}"
  exit 1
fi

# Check if executable exists
if [ ! -f "$BUILD_DIR/llm_visualizer" ]; then
  echo -e "${RED}Executable not found after build!${NC}"
  exit 1
fi

# Run the application
echo -e "${GREEN}Running LLM Visualizer...${NC}"
./llm_visualizer
if [ $? -ne 0 ]; then
  echo -e "${RED}Application crashed with error code $?${NC}"
  exit 1
fi

echo -e "${GREEN}Done!${NC}" 