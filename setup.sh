#!/bin/bash
# NeonWave Setup Script - Handles projectM compilation and dependencies

set -e

echo "🌊 NeonWave Setup Script"
echo "========================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check for required system packages
echo -e "${YELLOW}Checking system dependencies...${NC}"
REQUIRED_PKGS="qt6-base cmake sdl2 pipewire git gcc"

for pkg in $REQUIRED_PKGS; do
    if ! pacman -Qi $pkg &> /dev/null; then
        echo -e "${RED}Missing package: $pkg${NC}"
        echo "Please install with: sudo pacman -S $pkg"
        exit 1
    fi
done

echo -e "${GREEN}All system dependencies satisfied!${NC}"

# Create external directory for dependencies
mkdir -p external
cd external

# Clone and build projectM if not already present
if [ ! -d "projectm" ]; then
    echo -e "${YELLOW}Cloning projectM...${NC}"
    git clone --recursive https://github.com/projectM-visualizer/projectm.git
    cd projectm
    
    echo -e "${YELLOW}Building projectM...${NC}"
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_SDL_UI=ON
    make -j$(nproc)
    
    echo -e "${GREEN}projectM built successfully!${NC}"
    cd ../../..
else
    echo -e "${GREEN}projectM already exists, skipping clone${NC}"
fi

# Clone SDL frontend example for reference
if [ ! -d "frontend-sdl-cpp" ]; then
    echo -e "${YELLOW}Cloning projectM SDL frontend example...${NC}"
    cd external
    git clone https://github.com/projectM-visualizer/frontend-sdl-cpp.git
    cd ..
fi

# Create build directory
mkdir -p build

echo -e "${GREEN}Setup complete! You can now build NeonWave:${NC}"
echo "  cd build"
echo "  cmake .."
echo "  make -j\$(nproc)"
echo "  ./neonwave"