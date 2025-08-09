/**
 * @file ProjectMWidget.cpp
 * @brief Implementation of ProjectM OpenGL widget
 */

#include "ProjectMWidget.h"
#include <QTimer>
#include <QOpenGLContext>
#include <iostream>
#include <filesystem>

// ProjectM headers
#include <projectM-4/projectM.h>
#include <projectM-4/playlist.h>

namespace NeonWave::GUI {

/**
 * @class ProjectMWidget::Impl
 * @brief Private implementation containing ProjectM instance
 */
class ProjectMWidget::Impl {
public:
    projectm_handle projectM = nullptr;
    projectm_playlist_handle playlist = nullptr;
    QTimer* renderTimer = nullptr;
    bool initialized = false;
    bool presetLocked = false;
    std::string currentPresetName;
    
    ~Impl() {
        cleanup();
    }
    
    void cleanup() {
        if (playlist) {
            projectm_playlist_destroy(playlist);
            playlist = nullptr;
        }
        if (projectM) {
            projectm_destroy(projectM);
            projectM = nullptr;
        }
    }
};

ProjectMWidget::ProjectMWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , pImpl(std::make_unique<Impl>()) {
    
    // Set OpenGL format
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4); // Enable multisampling
    setFormat(format);
}

ProjectMWidget::~ProjectMWidget() {
    makeCurrent();
    cleanupProjectM();
    doneCurrent();
}

void ProjectMWidget::initializeGL() {
    std::cout << "[ProjectMWidget] Initializing OpenGL context" << std::endl;
    
    // Initialize OpenGL functions
    initializeOpenGLFunctions();
    
    // Set up OpenGL state
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize ProjectM
    if (!initializeProjectM()) {
        std::cerr << "[ProjectMWidget] Failed to initialize ProjectM!" << std::endl;
        return;
    }
    
    // Start render timer (60 FPS)
    startRenderTimer();
}

void ProjectMWidget::resizeGL(int w, int h) {
    if (pImpl->projectM) {
        projectm_set_window_size(pImpl->projectM, w, h);
    }
}

void ProjectMWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (pImpl->projectM && pImpl->initialized) {
        // Render ProjectM frame
        projectm_opengl_render_frame(pImpl->projectM);
    }
}

bool ProjectMWidget::initializeProjectM() {
    std::cout << "[ProjectMWidget] Initializing ProjectM..." << std::endl;
    
    // Set ProjectM settings
    projectm_settings settings = {};
    settings.window_width = width();
    settings.window_height = height();
    settings.fps = 60;
    settings.mesh_x = 32;
    settings.mesh_y = 24;
    settings.aspect_correction = true;
    settings.beat_sensitivity = 1.0f;
    settings.hard_cut_duration = 3.0f;
    settings.hard_cut_enabled = true;
    settings.soft_cut_duration = 10.0f;
    settings.preset_duration = 30.0f;
    
    // Default preset path
    std::string presetPath = "/usr/share/projectM/presets";
    if (!std::filesystem::exists(presetPath)) {
        // Try local build path
        presetPath = "./external/projectm/presets";
    }
    settings.preset_path = presetPath.c_str();
    
    // Default texture path
    std::string texturePath = "/usr/share/projectM/textures";
    if (!std::filesystem::exists(texturePath)) {
        texturePath = "./external/projectm/textures";
    }
    settings.texture_search_paths = texturePath.c_str();
    
    // Create ProjectM instance
    pImpl->projectM = projectm_create(&settings);
    if (!pImpl->projectM) {
        std::cerr << "[ProjectMWidget] Failed to create ProjectM instance!" << std::endl;
        return false;
    }
    
    // Create playlist
    pImpl->playlist = projectm_playlist_create(pImpl->projectM);
    if (!pImpl->playlist) {
        std::cerr << "[ProjectMWidget] Failed to create playlist!" << std::endl;
        return false;
    }
    
    // Load presets from directory
    if (std::filesystem::exists(presetPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(presetPath)) {
            if (entry.path().extension() == ".milk") {
                projectm_playlist_add_preset(pImpl->playlist, 
                                            entry.path().string().c_str(), 
                                            true);
            }
        }
        
        size_t presetCount = projectm_playlist_size(pImpl->playlist);
        std::cout << "[ProjectMWidget] Loaded " << presetCount << " presets" << std::endl;
        
        if (presetCount > 0) {
            projectm_playlist_set_position(pImpl->playlist, 0);
            projectm_playlist_play_next(pImpl->playlist, true);
        }
    }
    
    pImpl->initialized = true;
    return true;
}

void ProjectMWidget::cleanupProjectM() {
    if (pImpl->renderTimer) {
        pImpl->renderTimer->stop();
        delete pImpl->renderTimer;
        pImpl->renderTimer = nullptr;
    }
    
    pImpl->cleanup();
    pImpl->initialized = false;
}

void ProjectMWidget::startRenderTimer() {
    pImpl->renderTimer = new QTimer(this);
    connect(pImpl->renderTimer, &QTimer::timeout, [this]() {
        update(); // Trigger paintGL
    });
    pImpl->renderTimer->start(16); // ~60 FPS
}

bool ProjectMWidget::loadPreset(const std::string& presetPath) {
    if (!pImpl->projectM || !std::filesystem::exists(presetPath)) {
        return false;
    }
    
    projectm_load_preset_file(pImpl->projectM, presetPath.c_str());
    pImpl->currentPresetName = std::filesystem::path(presetPath).stem();
    emit presetChanged(QString::fromStdString(pImpl->currentPresetName));
    return true;
}

void ProjectMWidget::nextPreset() {
    if (pImpl->playlist && !pImpl->presetLocked) {
        projectm_playlist_play_next(pImpl->playlist, true);
        // Update current preset name
        size_t pos = projectm_playlist_get_position(pImpl->playlist);
        const char* name = projectm_playlist_item(pImpl->playlist, pos);
        if (name) {
            pImpl->currentPresetName = std::filesystem::path(name).stem();
            emit presetChanged(QString::fromStdString(pImpl->currentPresetName));
        }
    }
}

void ProjectMWidget::previousPreset() {
    if (pImpl->playlist && !pImpl->presetLocked) {
        projectm_playlist_play_previous(pImpl->playlist, true);
        // Update current preset name
        size_t pos = projectm_playlist_get_position(pImpl->playlist);
        const char* name = projectm_playlist_item(pImpl->playlist, pos);
        if (name) {
            pImpl->currentPresetName = std::filesystem::path(name).stem();
            emit presetChanged(QString::fromStdString(pImpl->currentPresetName));
        }
    }
}

void ProjectMWidget::randomPreset() {
    if (pImpl->playlist && !pImpl->presetLocked) {
        size_t count = projectm_playlist_size(pImpl->playlist);
        if (count > 0) {
            size_t randomPos = rand() % count;
            projectm_playlist_set_position(pImpl->playlist, randomPos);
            projectm_playlist_play_next(pImpl->playlist, true);
            
            const char* name = projectm_playlist_item(pImpl->playlist, randomPos);
            if (name) {
                pImpl->currentPresetName = std::filesystem::path(name).stem();
                emit presetChanged(QString::fromStdString(pImpl->currentPresetName));
            }
        }
    }
}

std::string ProjectMWidget::getCurrentPresetName() const {
    return pImpl->currentPresetName;
}

void ProjectMWidget::addAudioData(const float* pcmData, size_t samples) {
    if (pImpl->projectM && pImpl->initialized) {
        // ProjectM expects stereo interleaved float samples
        projectm_pcm_add_float(pImpl->projectM, pcmData, samples, 
                               PROJECTM_STEREO);
    }
}

void ProjectMWidget::setBPM(float bpm) {
    if (pImpl->projectM) {
        // ProjectM uses this for beat detection
        // Note: This is a simplified approach
        float beatSensitivity = bpm / 120.0f; // Normalize around 120 BPM
        projectm_set_beat_sensitivity(pImpl->projectM, beatSensitivity);
    }
}

void ProjectMWidget::setPresetLocked(bool locked) {
    pImpl->presetLocked = locked;
    if (pImpl->projectM) {
        projectm_set_preset_locked(pImpl->projectM, locked);
    }
}

} // namespace NeonWave::GUI