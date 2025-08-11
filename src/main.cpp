/**
 * @file main.cpp
 * @brief NeonWave application entry point
 * 
 * This file initializes the Qt application and launches the main window.
 * Designed for clarity and AI-assisted development.
 */

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <memory>

#include "core/Application.h"
#include "gui/MainWindow.h"

/**
 * @brief Application entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit code
 */
int main(int argc, char *argv[]) {
    // Initialize Qt application
    QApplication qtApp(argc, argv);
    
    // Set application metadata for QSettings
    QCoreApplication::setOrganizationName("NeonWave");
    QCoreApplication::setApplicationName("NeonWave");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    try {
        // Initialize core application and load config
        auto app = std::make_unique<NeonWave::Core::Application>();
        app->initialize();
        
        // Create and show main window
        auto mainWindow = std::make_unique<NeonWave::GUI::MainWindow>();
        mainWindow->show();
        
        // Run Qt event loop
        return qtApp.exec();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}