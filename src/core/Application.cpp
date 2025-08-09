/**
 * @file Application.cpp
 * @brief Implementation of core application management
 */

#include "Application.h"
#include <QStandardPaths>
#include <QDir>
#include <iostream>
#include <fstream>

namespace NeonWave::Core {

// Static instance pointer
Application* Application::s_instance = nullptr;

/**
 * @class Application::Impl
 * @brief Private implementation (PIMPL pattern)
 */
class Application::Impl {
public:
    bool initialized = false;
    std::filesystem::path configPath;
    std::filesystem::path dataPath;
    
    Impl() {
        // Set up paths using Qt's standard paths
        QString configDir = QStandardPaths::writableLocation(
            QStandardPaths::ConfigLocation) + "/neonwave";
        configPath = configDir.toStdString();
        
        QString dataDir = QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation);
        dataPath = dataDir.toStdString();
    }
};

Application::Application() : pImpl(std::make_unique<Impl>()) {
    s_instance = this;
    ensureConfigDirectories();
}

Application::~Application() {
    shutdown();
    s_instance = nullptr;
}

Application& Application::instance() {
    if (!s_instance) {
        throw std::runtime_error("Application not initialized");
    }
    return *s_instance;
}

std::filesystem::path Application::getConfigPath() const {
    return pImpl->configPath;
}

std::filesystem::path Application::getDataPath() const {
    return pImpl->dataPath;
}

bool Application::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
    std::cout << "[NeonWave] Initializing application..." << std::endl;
    std::cout << "[NeonWave] Config path: " << pImpl->configPath << std::endl;
    
    // Create default configuration files if they don't exist
    auto settingsPath = pImpl->configPath / "settings.json";
    if (!std::filesystem::exists(settingsPath)) {
        std::ofstream settings(settingsPath);
        settings << R"({
    "audio": {
        "volume": 0.7,
        "crossfade": 0
    },
    "visualizer": {
        "fps": 60,
        "fullscreen": false,
        "preset_duration": 30
    },
    "interface": {
        "theme": "dark"
    }
})" << std::endl;
    }
    
    // Create empty favorites and blacklist files
    auto favoritesPath = pImpl->configPath / "favorites.json";
    if (!std::filesystem::exists(favoritesPath)) {
        std::ofstream favorites(favoritesPath);
        favorites << "[]" << std::endl;
    }
    
    auto blacklistPath = pImpl->configPath / "blacklist.json";
    if (!std::filesystem::exists(blacklistPath)) {
        std::ofstream blacklist(blacklistPath);
        blacklist << "[]" << std::endl;
    }
    
    pImpl->initialized = true;
    return true;
}

void Application::shutdown() {
    if (!pImpl->initialized) {
        return;
    }
    
    std::cout << "[NeonWave] Shutting down application..." << std::endl;
    pImpl->initialized = false;
}

void Application::ensureConfigDirectories() {
    // Create config directory if it doesn't exist
    if (!std::filesystem::exists(pImpl->configPath)) {
        std::filesystem::create_directories(pImpl->configPath);
    }
    
    // Create data directory if it doesn't exist
    if (!std::filesystem::exists(pImpl->dataPath)) {
        std::filesystem::create_directories(pImpl->dataPath);
    }
}

} // namespace NeonWave::Core