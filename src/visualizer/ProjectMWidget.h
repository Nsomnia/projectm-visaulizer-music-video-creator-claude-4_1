/**
 * @file ProjectMWidget.h
 * @brief Qt widget wrapping ProjectM visualizer
 */

#pragma once

#include <QOpenGLWidget>
#include <memory>
#include <string>

namespace NeonWave::GUI {

/**
 * @class ProjectMWidget
 * @brief OpenGL widget that renders ProjectM visualizations
 * 
 * This widget integrates ProjectM into Qt's OpenGL rendering system,
 * providing audio visualization with MilkDrop preset support.
 */
class ProjectMWidget : public QOpenGLWidget {
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
    
    /**
     * @brief Feed audio data to visualizer
     * @param pcmData Audio samples
     * @param samples Number of samples
     */
    void addAudioData(const float* pcmData, size_t samples);
    
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
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
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
     * @brief Start render timer
     */
    void startRenderTimer();
};

} // namespace NeonWave::GUI