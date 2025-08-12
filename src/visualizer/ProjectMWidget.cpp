/**
 * @file ProjectMWidget.cpp
 * @brief Implementation of ProjectM OpenGL widget
 */

#include "ProjectMWidget.h"
#include <QTimer>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
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

class ProjectMWidget::Impl {
public:
    projectm_handle projectM = nullptr;
    projectm_playlist_handle playlist = nullptr;
    QTimer* renderTimer = nullptr;
    bool initialized = false;
    bool presetLocked = false;
    std::string currentPresetName;
    std::recursive_mutex projectm_mutex;
    
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
    , pImpl(std::make_unique<Impl>())
    , m_fboRenderer(std::make_unique<FboRenderer>())
    , m_videoExporter(std::make_unique<VideoExporter>())
    , m_textRenderer(std::make_unique<TextRenderer>()) {
    
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    setFormat(format);
}

ProjectMWidget::~ProjectMWidget() {
    makeCurrent();
    if (m_shaderProgram) {
        delete m_shaderProgram;
    }
    if (m_quadVao) {
        glDeleteVertexArrays(1, &m_quadVao);
    }
    if (m_quadVbo) {
        glDeleteBuffers(1, &m_quadVbo);
    }
    if (m_quadEbo) {
        glDeleteBuffers(1, &m_quadEbo);
    }
    cleanupProjectM();
    doneCurrent();
}

void ProjectMWidget::initializeGL() {
    std::cout << "[ProjectMWidget] Initializing OpenGL context" << std::endl;
    
    initializeOpenGLFunctions();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    if (!m_fboRenderer->init(width(), height())) {
        std::cerr << "[ProjectMWidget] Failed to initialize FBO Renderer!" << std::endl;
        return;
    }

    initShaderProgram();
    initRenderQuad();

    const auto& textOverlayConfig = Core::Config::instance().visualizer().textOverlay;
    if (!m_textRenderer->init(textOverlayConfig.fontPath, textOverlayConfig.fontSize)) {
        std::cerr << "[ProjectMWidget] Failed to initialize Text Renderer!" << std::endl;
    }

    if (!initializeProjectM()) {
        std::cerr << "[ProjectMWidget] Failed to initialize ProjectM!" << std::endl;
        return;
    }
    
    startRenderTimer();
}

void ProjectMWidget::resizeGL(int w, int h) {
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        projectm_set_window_size(pImpl->projectM, w, h);
    }
    m_fboRenderer->init(w, h);
    m_textRenderer->setProjection(w, h);
}

void ProjectMWidget::paintGL() {
    if (pImpl->projectM && pImpl->initialized) {
        try {
            std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);

            m_fboRenderer->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            projectm_opengl_render_frame(pImpl->projectM);
            m_fboRenderer->release();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_shaderProgram->bind();
            m_shaderProgram->setUniformValue("screenTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_fboRenderer->texture());
            glBindVertexArray(m_quadVao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            m_shaderProgram->release();

            const auto& textOverlayConfig = Core::Config::instance().visualizer().textOverlay;
            if (textOverlayConfig.show) {
                m_textRenderer->renderText(pImpl->currentPresetName, 10.0f, 10.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            }

        } catch (const std::exception& e) {
            std::cerr << "[ProjectMWidget] Render error: " << e.what() << std::endl;
        }
    }

    if (m_videoExporter->isRecording()) {
        std::vector<unsigned char> frame_buffer(width() * height() * 3);
        glReadPixels(0, 0, width(), height(), GL_RGB, GL_UNSIGNED_BYTE, frame_buffer.data());
        m_videoExporter->writeFrame(frame_buffer);
    }
}

void ProjectMWidget::startRecording(const std::string& outputPath) {
    m_videoExporter->start(width(), height(), 60, outputPath);
}

void ProjectMWidget::stopRecording() {
    m_videoExporter->stop();
}

void ProjectMWidget::initShaderProgram() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;
        out vec2 TexCoords;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
            TexCoords = aTexCoords;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D screenTexture;
        void main() {
            FragColor = texture(screenTexture, TexCoords);
        }
    )";

    m_shaderProgram = new QOpenGLShaderProgram(this);
    if (!m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        std::cerr << "Vertex shader compilation error: " << m_shaderProgram->log().toStdString() << std::endl;
        return;
    }
    if (!m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        std::cerr << "Fragment shader compilation error: " << m_shaderProgram->log().toStdString() << std::endl;
        return;
    }
    if (!m_shaderProgram->link()) {
        std::cerr << "Shader program linking error: " << m_shaderProgram->log().toStdString() << std::endl;
        return;
    }
}

void ProjectMWidget::initRenderQuad() {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &m_quadVao);
    glGenBuffers(1, &m_quadVbo);
    glGenBuffers(1, &m_quadEbo);

    glBindVertexArray(m_quadVao);

    glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

bool ProjectMWidget::initializeProjectM() {
    std::cout << "[ProjectMWidget] Initializing ProjectM..." << std::endl;
    
    // Create ProjectM instance (v4 API)
    pImpl->projectM = projectm_create();
    if (!pImpl->projectM) {
        std::cerr << "[ProjectMWidget] Failed to create ProjectM instance!" << std::endl;
        return false;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);

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

    // Register preset switch callback
    projectm_set_preset_switch_requested_event_callback(pImpl->projectM, presetSwitchedCallback, this);

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
    std::cout << "[ProjectMWidget] Searching for presets in: " << presetPath << std::endl;
    if (std::filesystem::exists(presetPath)) {
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(presetPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".milk") {
                    if (!Visualizer::PresetManager::instance().isBlacklisted(entry.path().stem().string())) {
                        discoveredPresets.emplace_back(entry.path().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[ProjectMWidget] Filesystem error while scanning presets: " << e.what() << std::endl;
        }
        presetCount = discoveredPresets.size();
    }
    std::cout << "[ProjectMWidget] Found " << presetCount << " presets" << std::endl;
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
    
    const auto& vcfg = NeonWave::Core::Config::instance().visualizer();
    if (vcfg.loadRandomPresetOnStartup && presetCount > 0) {
        randomPreset();
    } else {
        // Load default idle preset for initial visualization when no audio is playing
        projectm_load_preset_file(pImpl->projectM, "idle://", true);
        pImpl->currentPresetName = "Idle";
        emit presetChanged(QString::fromStdString(pImpl->currentPresetName));
    }
    
    pImpl->initialized = true;
    return true;
}

void ProjectMWidget::cleanupProjectM() {
    std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
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
    
    std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
    projectm_load_preset_file(pImpl->projectM, presetPath.c_str(), true);
    pImpl->currentPresetName = std::filesystem::path(presetPath).stem();
    emit presetChanged(QString::fromStdString(pImpl->currentPresetName));
    return true;
}

void ProjectMWidget::nextPreset() {
    if (pImpl->playlist && !pImpl->presetLocked) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        unsigned int new_index = projectm_playlist_play_next(pImpl->playlist, true);
        onPresetSwitched(new_index);
    }
}

void ProjectMWidget::previousPreset() {
    if (pImpl->playlist && !pImpl->presetLocked) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        unsigned int new_index = projectm_playlist_play_previous(pImpl->playlist, true);
        onPresetSwitched(new_index);
    }
}

void ProjectMWidget::randomPreset() {
    if (pImpl->playlist && !pImpl->presetLocked) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        size_t count = projectm_playlist_size(pImpl->playlist);
        if (count > 0) {
            size_t randomPos = rand() % count;
            unsigned int new_index = projectm_playlist_set_position(pImpl->playlist, randomPos, true);
            onPresetSwitched(new_index);
        }
    }
}

std::string ProjectMWidget::getCurrentPresetName() const {
    return pImpl->currentPresetName;
}

void ProjectMWidget::addAudioData(const QByteArray& data, int sampleCount, int channelCount) {
    if (pImpl->projectM && pImpl->initialized && channelCount > 0) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        const float* pcmData = reinterpret_cast<const float*>(data.constData());
        size_t frames = sampleCount;
        auto channels = static_cast<projectm_channels>(channelCount);
        projectm_pcm_add_float(pImpl->projectM, pcmData, frames, channels);
    }
}


void ProjectMWidget::setBPM(float bpm) {
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        // ProjectM uses this for beat detection
        // Note: This is a simplified approach
        float beatSensitivity = bpm / 120.0f; // Normalize around 120 BPM
        projectm_set_beat_sensitivity(pImpl->projectM, beatSensitivity);
    }
}

void ProjectMWidget::setPresetLocked(bool locked) {
    pImpl->presetLocked = locked;
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        projectm_set_preset_locked(pImpl->projectM, locked);
    }
}

void ProjectMWidget::setFPS(int fps) {
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        projectm_set_fps(pImpl->projectM, fps);
    }
}

void ProjectMWidget::setMeshSize(int x, int y) {
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        projectm_set_mesh_size(pImpl->projectM, x, y);
    }
}

void ProjectMWidget::setAspectCorrection(bool enabled) {
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        projectm_set_aspect_correction(pImpl->projectM, enabled);
    }
}

void ProjectMWidget::setBeatSensitivity(float sensitivity) {
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        projectm_set_beat_sensitivity(pImpl->projectM, sensitivity);
    }
}

void ProjectMWidget::setHardCut(bool enabled, double durationSeconds) {
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        projectm_set_hard_cut_enabled(pImpl->projectM, enabled);
        projectm_set_hard_cut_duration(pImpl->projectM, durationSeconds);
    }
}

void ProjectMWidget::setSoftCutDuration(double durationSeconds) {
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        projectm_set_soft_cut_duration(pImpl->projectM, durationSeconds);
    }
}

void ProjectMWidget::setPresetDuration(double seconds) {
    if (pImpl->projectM) {
        std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
        projectm_set_preset_duration(pImpl->projectM, seconds);
    }
}

void ProjectMWidget::setPresetAndTextureDirs(const std::string& presetDir, const std::string& textureDir) {
    if (!pImpl->projectM) return;
    std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
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
    std::cout << "[ProjectMWidget] Setting texture search path to: " << texturePath << std::endl;
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
    std::cout << "[ProjectMWidget] Rebuilding playlist from: " << presetPath << std::endl;
    if (std::filesystem::exists(presetPath)) {
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(presetPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".milk") {
                    if (!Visualizer::PresetManager::instance().isBlacklisted(entry.path().stem().string())) {
                        newPresets.emplace_back(entry.path().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[ProjectMWidget] Filesystem error while scanning presets: " << e.what() << std::endl;
        }
        count = newPresets.size();
    }
    std::cout << "[ProjectMWidget] Found " << count << " presets" << std::endl;
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

void ProjectMWidget::presetSwitchedCallback(bool /*isHardCut*/, void* context)
{
    // This is a static C-style callback, so we use the context pointer
    // to call a member function on the correct class instance.
    if (context) {
        auto* that = static_cast<ProjectMWidget*>(context);
        // The callback can be called from a different thread, so we need to
        // queue the call to the main thread to safely update the UI.
        unsigned int index = projectm_playlist_get_position(that->pImpl->playlist);
        QMetaObject::invokeMethod(that, [that, index]() {
            that->onPresetSwitched(index);
        }, Qt::QueuedConnection);
    }
}

void ProjectMWidget::onPresetSwitched(unsigned int index)
{
    std::lock_guard<std::recursive_mutex> lock(pImpl->projectm_mutex);
    if (pImpl->playlist) {
        char* name_ptr = projectm_playlist_item(pImpl->playlist, index);
        if (name_ptr) {
            // projectM returns a pointer to its internal string. We must copy it.
            std::string preset_path = name_ptr;
            projectm_playlist_free_string(name_ptr);
            pImpl->currentPresetName = std::filesystem::path(preset_path).stem().string();
            emit presetChanged(QString::fromStdString(pImpl->currentPresetName));
        }
    }
}

} // namespace NeonWave::GUI