/**
 * @file ProjectMWidget.cpp
 * @brief Implementation of ProjectM OpenGL widget
 */

#include "ProjectMWidget.h"
#include <QTimer>
#include <QOpenGLContext>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include "core/Config.h"

// ProjectM headers
#include <projectM-4/projectM.h>
#include <projectM-4/playlist.h>
#include <projectM-4/parameters.h>
#include <projectM-4/render_opengl.h>

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
    // Some drivers/presets assume compatibility profile
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
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
        // Render ProjectM frame (guard against upstream exceptions)
        try {
            // Optional: inject a small test signal for debug visibility
            const auto& vcfg = NeonWave::Core::Config::instance().visualizer();
            if (vcfg.debugInjectTestSignal) {
                constexpr size_t frames = 512;
                static float phase = 0.0f;
                float buffer[frames * 2];
                const float freq = 220.0f;
                const float sampleRate = 48000.0f;
                for (size_t i = 0; i < frames; ++i) {
                    float s = 0.1f * sinf(2.0f * 3.14159265f * freq * (phase / sampleRate));
                    buffer[2 * i] = s;
                    buffer[2 * i + 1] = s;
                    phase += 1.0f;
                }
                projectm_pcm_add_float(pImpl->projectM, buffer, frames * 2, PROJECTM_STEREO);
            }

            // Render into the widget's default FBO
            projectm_opengl_render_frame_fbo(pImpl->projectM, static_cast<uint32_t>(defaultFramebufferObject()));
        } catch (const std::exception& e) {
            std::cerr << "[ProjectMWidget] Render error: " << e.what() << std::endl;
        }
    }
}

bool ProjectMWidget::initializeProjectM() {
    std::cout << "[ProjectMWidget] Initializing ProjectM..." << std::endl;
    
    // Create ProjectM instance (v4 API)
    pImpl->projectM = projectm_create();
    if (!pImpl->projectM) {
        std::cerr << "[ProjectMWidget] Failed to create ProjectM instance!" << std::endl;
        return false;
    }

    // Apply parameters via API
    projectm_set_window_size(pImpl->projectM, static_cast<size_t>(width()), static_cast<size_t>(height()));
    projectm_set_fps(pImpl->projectM, 60);
    projectm_set_mesh_size(pImpl->projectM, 32, 24);
    projectm_set_aspect_correction(pImpl->projectM, true);
    projectm_set_beat_sensitivity(pImpl->projectM, 1.0f);
    projectm_set_hard_cut_duration(pImpl->projectM, 3.0);
    projectm_set_hard_cut_enabled(pImpl->projectM, true);
    projectm_set_soft_cut_duration(pImpl->projectM, 10.0);
    projectm_set_preset_duration(pImpl->projectM, 30.0);

    // Texture search paths
    std::string texturePath = "/usr/share/projectM/textures";
#ifdef PROJECTM_DEFAULT_TEXTURES_DIR
    if (!std::filesystem::exists(texturePath)) {
        texturePath = PROJECTM_DEFAULT_TEXTURES_DIR;
    }
#else
    if (!std::filesystem::exists(texturePath)) {
        texturePath = "./external/projectm/textures";
    }
#endif
    const char* texturePaths[] = { texturePath.c_str() };
    projectm_set_texture_search_paths(pImpl->projectM, texturePaths, 1);
    
    // Discover presets from directory (if present)
    std::string presetPath = "/usr/share/projectM/presets";
#ifdef PROJECTM_DEFAULT_PRESETS_DIR
    if (!std::filesystem::exists(presetPath)) {
        presetPath = PROJECTM_DEFAULT_PRESETS_DIR;
    }
#else
    if (!std::filesystem::exists(presetPath)) {
        presetPath = "./external/projectm/presets";
    }
#endif
    size_t presetCount = 0;
    std::vector<std::string> discoveredPresets;
    if (std::filesystem::exists(presetPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(presetPath)) {
            if (entry.path().extension() == ".milk") {
                discoveredPresets.emplace_back(entry.path().string());
            }
        }
        presetCount = discoveredPresets.size();
    }
    std::cout << "[ProjectMWidget] Loaded " << presetCount << " presets" << std::endl;
    // Do not auto-switch; keep idle preset initially for debug visibility
    if (presetCount > 0) {
        pImpl->playlist = projectm_playlist_create(pImpl->projectM);
        if (pImpl->playlist) {
            for (const auto& path : discoveredPresets) {
                projectm_playlist_add_preset(pImpl->playlist, path.c_str(), true);
            }
            projectm_playlist_set_position(pImpl->playlist, 0, false);
            projectm_set_preset_locked(pImpl->projectM, false);
        }
    }
    
    // Load default idle preset for initial visualization when no audio is playing
    projectm_load_preset_file(pImpl->projectM, "idle://", true);
    pImpl->currentPresetName = "Idle";
    emit presetChanged(QString::fromStdString(pImpl->currentPresetName));
    
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
    
    projectm_load_preset_file(pImpl->projectM, presetPath.c_str(), true);
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
            projectm_playlist_set_position(pImpl->playlist, randomPos, true);
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

void ProjectMWidget::setFPS(int fps) {
    if (pImpl->projectM) projectm_set_fps(pImpl->projectM, fps);
}

void ProjectMWidget::setMeshSize(int x, int y) {
    if (pImpl->projectM) projectm_set_mesh_size(pImpl->projectM, x, y);
}

void ProjectMWidget::setAspectCorrection(bool enabled) {
    if (pImpl->projectM) projectm_set_aspect_correction(pImpl->projectM, enabled);
}

void ProjectMWidget::setBeatSensitivity(float sensitivity) {
    if (pImpl->projectM) projectm_set_beat_sensitivity(pImpl->projectM, sensitivity);
}

void ProjectMWidget::setHardCut(bool enabled, double durationSeconds) {
    if (pImpl->projectM) {
        projectm_set_hard_cut_enabled(pImpl->projectM, enabled);
        projectm_set_hard_cut_duration(pImpl->projectM, durationSeconds);
    }
}

void ProjectMWidget::setSoftCutDuration(double durationSeconds) {
    if (pImpl->projectM) projectm_set_soft_cut_duration(pImpl->projectM, durationSeconds);
}

void ProjectMWidget::setPresetDuration(double seconds) {
    if (pImpl->projectM) projectm_set_preset_duration(pImpl->projectM, seconds);
}

void ProjectMWidget::setPresetAndTextureDirs(const std::string& presetDir, const std::string& textureDir) {
    if (!pImpl->projectM) return;
    // Update textures search paths
    std::string texturePath = textureDir;
    if (texturePath.empty()) {
        texturePath = "/usr/share/projectM/textures";
#ifdef PROJECTM_DEFAULT_TEXTURES_DIR
        if (!std::filesystem::exists(texturePath)) texturePath = PROJECTM_DEFAULT_TEXTURES_DIR;
#else
        if (!std::filesystem::exists(texturePath)) texturePath = "./external/projectm/textures";
#endif
    }
    const char* texturePaths[] = { texturePath.c_str() };
    projectm_set_texture_search_paths(pImpl->projectM, texturePaths, 1);

    // Rebuild playlist from new preset directory if provided
    if (pImpl->playlist) {
        projectm_playlist_destroy(pImpl->playlist);
        pImpl->playlist = nullptr;
    }

    std::string presetPath = presetDir;
    if (presetPath.empty()) {
        presetPath = "/usr/share/projectM/presets";
#ifdef PROJECTM_DEFAULT_PRESETS_DIR
        if (!std::filesystem::exists(presetPath)) presetPath = PROJECTM_DEFAULT_PRESETS_DIR;
#else
        if (!std::filesystem::exists(presetPath)) presetPath = "./external/projectm/presets";
#endif
    }
    size_t count = 0;
    std::vector<std::string> newPresets;
    if (std::filesystem::exists(presetPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(presetPath)) {
            if (entry.path().extension() == ".milk") {
                newPresets.emplace_back(entry.path().string());
            }
        }
        count = newPresets.size();
    }
    if (count > 0) {
        pImpl->playlist = projectm_playlist_create(pImpl->projectM);
        if (!pImpl->playlist) return;
        for (const auto& path : newPresets) {
            projectm_playlist_add_preset(pImpl->playlist, path.c_str(), true);
        }
        projectm_playlist_set_position(pImpl->playlist, 0, false);
        projectm_set_preset_locked(pImpl->projectM, false);
    } else {
        // leave playlist as nullptr and keep idle preset
        projectm_set_preset_locked(pImpl->projectM, true);
    }
}

} // namespace NeonWave::GUI