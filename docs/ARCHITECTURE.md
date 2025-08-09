# NeonWave Architecture Documentation

## Overview

NeonWave follows a modular Model-View-Controller (MVC) architecture optimized for AI-assisted development and maintenance.

## Core Components

### 1. Application Core (`src/core/`)
- **Application**: Singleton managing application lifecycle
- **Config**: Configuration management and persistence
- **Purpose**: Centralized application state and services

### 2. GUI Layer (`src/gui/`)
- **MainWindow**: Primary application window
- **ProjectMWidget**: OpenGL widget for visualization
- **PlaylistWidget**: Playlist management interface
- **SettingsDialog**: Configuration interface
- **Purpose**: User interface and interaction handling

### 3. Audio Engine (`src/audio/`)
- **AudioEngine**: Audio playback and processing
- **AudioDecoder**: Format decoding support
- **Purpose**: Audio file handling and streaming

### 4. Visualizer (`src/visualizer/`)
- **ProjectMWidget**: ProjectM integration
- **PresetManager**: Preset favorites and blacklist
- **Purpose**: Visualization rendering and preset management

### 5. Utilities (`src/utils/`)
- **FileUtils**: File system operations
- **StringUtils**: String manipulation helpers
- **MathUtils**: Mathematical operations
- **Purpose**: Reusable helper functions

## Data Flow

1. **Audio Input** → AudioEngine → ProjectM → OpenGL Rendering
2. **User Input** → GUI Events → Controllers → Model Updates
3. **Configuration** → Config Manager → Persistent Storage

## Design Principles

### AI-Friendly Development
- Extensive inline documentation
- Clear function signatures
- Modular, single-responsibility components
- Consistent naming conventions

### Extensibility
- Plugin-ready architecture
- Clear interfaces between components
- Event-driven communication
- Dependency injection where appropriate

### Performance
- Efficient audio streaming
- GPU-accelerated visualization
- Minimal UI thread blocking
- Smart pointer usage for memory management

## File Organization
    src/
    ├── core/ # Application core logic
    ├── gui/ # User interface components
    ├── audio/ # Audio processing
    ├── visualizer/ # Visualization logic
    └── utils/ # Helper utilities

## Configuration Storage

User settings stored in `~/.config/neonwave/`:
- `settings.json`: Application preferences
- `favorites.json`: Favorite presets list
- `blacklist.json`: Blacklisted presets
- `playlists/`: Saved playlists

## Threading Model

- **Main Thread**: GUI and event handling
- **Audio Thread**: Audio decoding and streaming
- **Render Thread**: OpenGL visualization rendering

## Error Handling

- Exceptions for critical errors
- Optional/Expected for recoverable errors
- Logging for debugging and monitoring
- Graceful degradation for missing features