/**
 * @file ProjectMWidget.h
 * @brief Qt widget wrapping ProjectM visualizer
 */

#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <memory>
#include <string>
#include <mutex>

#include "FboRenderer.h"
#include "VideoExporter.h"
#include "TextRenderer.h"

class QOpenGLShaderProgram;

namespace NeonWave::GUI {

/**
 * @class ProjectMWidget
 * @brief OpenGL widget that renders ProjectM visualizations
 * 
 * This widget integrates ProjectM into Qt's OpenGL rendering system,
 * providing audio visualization with MilkDrop preset support.
 */
class ProjectMWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
    
public:
    explicit ProjectMWidget(QWidget* parent = nullptr);
    ~ProjectMWidget();
    
    /**
     * @brief Load a specific preset file
     * @param presetPath Path to .milk preset file
     * @return true if successful
     */
    bool loadPreset(const std::string& presetPath);
    
    /**
     * @brief Switch to next preset in the collection
     */
    void nextPreset();
    
    /**
     * @brief Switch to previous preset
     */
    void previousPreset();
    
    /**
     * @brief Switch to a random preset
     */
    void randomPreset();
    
    /**
     * @brief Get current preset name
     * @return Name of the active preset
     */
    std::string getCurrentPresetName() const;

    void startRecording(const std::string& outputPath);
    void stopRecording();
    
    public slots:
    /**
     * @brief Add audio data to ProjectM for visualization
     * @param pcmData Raw float PCM data
     * @param samples Total number of samples (frames * channels)
     */
    void addAudioData(const QByteArray& data, int sampleCount, int channelCount);

    
    
    /**
     * @brief Set beats per minute (for beat detection)
     * @param bpm Beats per minute
     */
    void setBPM(float bpm);
    
    /**
     * @brief Toggle preset lock (prevent auto-switching)
     * @param locked true to lock current preset
     */
    void setPresetLocked(bool locked);

    // Settings application (from Config/SettingsDialog)
    void setFPS(int fps);
    void setMeshSize(int x, int y);
    void setAspectCorrection(bool enabled);
    void setBeatSensitivity(float sensitivity);
    void setHardCut(bool enabled, double durationSeconds);
    void setSoftCutDuration(double durationSeconds);
    void setPresetDuration(double seconds);
    void setPresetAndTextureDirs(const std::string& presetDir, const std::string& textureDir);
    
signals:
    /**
     * @brief Emitted when preset changes
     * @param presetName Name of new preset
     */
    void presetChanged(const QString& presetName);
    
protected:
    // OpenGL functions
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    
private:
    // projectM callback
    static void presetSwitchedCallback(bool isHardCut, unsigned int index, void* context);
    void onPresetSwitched(unsigned int index);

    class Impl;
    std::unique_ptr<Impl> pImpl;

    std::unique_ptr<FboRenderer> m_fboRenderer;
    std::unique_ptr<VideoExporter> m_videoExporter;
    std::unique_ptr<TextRenderer> m_textRenderer;
    QOpenGLShaderProgram* m_shaderProgram = nullptr;
    GLuint m_quadVao = 0;
    GLuint m_quadVbo = 0;
    GLuint m_quadEbo = 0;
    
    /**
     * @brief Initialize ProjectM instance
     * @return true if successful
     */
    bool initializeProjectM();
    
    /**
     * @brief Clean up ProjectM resources
     */
    void cleanupProjectM();

    /**
     * @brief Initializes the shader program for rendering the FBO texture.
     */
    void initShaderProgram();

    /**
     * @brief Initializes the geometry for rendering the FBO texture.
     */
    void initRenderQuad();
    
    /**
     * @brief Start render timer
     */
    void startRenderTimer();
};

} // namespace NeonWave::GUI