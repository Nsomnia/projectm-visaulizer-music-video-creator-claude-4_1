# 🌊 NeonWave - Advanced Audio Visualizer

<div align="center">
  
  ![License](https://img.shields.io/badge/license-GPL3-blue)
  ![C++](https://img.shields.io/badge/C++-20-green)
  ![Platform](https://img.shields.io/badge/platform-Linux-orange)
  
</div>

## ✨ Features

- **ProjectM Integration**: Stunning MilkDrop-compatible visualizations
- **Advanced Audio Playback**: Support for multiple audio formats
- **Playlist Management**: Drag-and-drop playlist with reordering
- **Preset Control**: Favorite and blacklist presets
- **Modern UI**: Clean Qt6-based interface
- **Extensible Architecture**: Designed for AI-assisted development

## 🚀 Quick Start

```bash
# Clone with submodules
git clone --recursive https://github.com/yourusername/neonwave.git
cd neonwave

# Run setup script
./setup.sh

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./neonwave