/**
 * @file Application.h
 * @brief Core application class managing lifecycle and configuration
 */

#pragma once

#include <memory>
#include <string>
#include <filesystem>

namespace NeonWave::Core {

/**
 * @class Application
 * @brief Manages application lifecycle, configuration, and core services
 * 
 * This singleton class initializes all core services and manages
 * the application configuration.
 */
class Application {
public:
    Application();
    ~Application();
    
    // Delete copy/move constructors for singleton
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    /**
     * @brief Get application instance
     * @return Singleton instance
     */
    static Application& instance();
    
    /**
     * @brief Get configuration directory path
     * @return Path to ~/.config/neonwave/
     */
    std::filesystem::path getConfigPath() const;
    
    /**
     * @brief Get application data directory
     * @return Path to application data
     */
    std::filesystem::path getDataPath() const;
    
    /**
     * @brief Initialize application services
     * @return true on success
     */
    bool initialize();
    
    /**
     * @brief Shutdown application services
     */
    void shutdown();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    static Application* s_instance;
    
    /**
     * @brief Create configuration directories if they don't exist
     */
    void ensureConfigDirectories();
};

} // namespace NeonWave::Core