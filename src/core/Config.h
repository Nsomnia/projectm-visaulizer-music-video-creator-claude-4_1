#pragma once

#include <string>
#include <filesystem>
#include <optional>

namespace NeonWave::Core {

struct RecordingConfig {
    std::string outputDirectory;
    int framerate = 60;
};

struct TextOverlayConfig {
    std::string fontPath;
    int fontSize = 48;
    bool show = true;
};

struct VisualizerConfig {
    int fps = 60;
    int meshX = 32;
    int meshY = 24;
    bool aspectCorrection = true;
    float beatSensitivity = 1.0f;
    bool hardCutEnabled = true;
    double hardCutDuration = 3.0;
    double softCutDuration = 10.0;
    double presetDuration = 30.0;
    bool presetLocked = false;
    std::string presetDirectory;   // empty => use defaults
    std::string textureDirectory;  // empty => use defaults
    bool debugInjectTestSignal = false; // developer toggle to verify rendering path
    bool loadRandomPresetOnStartup = false;

    RecordingConfig recording;
    TextOverlayConfig textOverlay;
};

struct AudioConfig {
    double volume = 0.7;
};

class Config {
public:
    static Config& instance();

    // Load/save settings from ~/.config/neonwave/
    void load();
    void save() const;

    // Accessors
    VisualizerConfig& visualizer();
    const VisualizerConfig& visualizer() const;

    AudioConfig& audio();
    const AudioConfig& audio() const;

    // Paths
    std::filesystem::path settingsFilePath() const;
    std::filesystem::path favoritesFilePath() const;
    std::filesystem::path blacklistFilePath() const;

private:
    Config() = default;

    VisualizerConfig m_visualizer;
    AudioConfig m_audio;
};

} // namespace NeonWave::Core
