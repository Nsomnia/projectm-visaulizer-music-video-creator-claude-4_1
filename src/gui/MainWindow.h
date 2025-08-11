/**
 * @file MainWindow.h
 * @brief Main application window containing all UI elements
 */

#pragma once

#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
class QAction;
class QToolBar;
class QSlider;
class QPushButton;
class QLabel;
class QListWidget;
QT_END_NAMESPACE

namespace NeonWave::GUI {

class ProjectMWidget;
class PlaylistWidget;

/**
 * @class MainWindow
 * @brief Primary application window managing layout and interactions
 * 
 * This window contains:
 * - ProjectM visualizer widget (center)
 * - Audio controls toolbar (bottom)
 * - Playlist sidebar (right)
 * - Menu bar with File, Settings, Help
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
signals:
    /**
     * @brief Emitted when play/pause state changes
     * @param playing true if playing, false if paused
     */
    void playStateChanged(bool playing);
    
    /**
     * @brief Emitted when user seeks to position
     * @param position Position in seconds
     */
    void seekRequested(double position);
    
    /**
     * @brief Emitted when volume changes
     * @param volume Volume level (0.0 - 1.0)
     */
    void volumeChanged(double volume);
    
private slots:
    /**
     * @brief Handle play/pause button toggle
     */
    void onPlayPauseToggled();
    
    /**
     * @brief Handle stop button click
     */
    void onStopClicked();
    
    /**
     * @brief Handle next track button
     */
    void onNextTrackClicked();
    
    /**
     * @brief Handle previous track button
     */
    void onPreviousTrackClicked();
    
    /**
     * @brief Open file dialog to add audio files
     */
    void onAddFilesClicked();
    
    /**
     * @brief Open settings dialog
     */
    void onSettingsClicked();
    
    /**
     * @brief Show about dialog
     */
    void onAboutClicked();
    
    /**
     * @brief Toggle preset as favorite
     */
    void onToggleFavoritePreset();
    
    /**
     * @brief Blacklist current preset
     */
    void onBlacklistPreset();
    
    /**
     * @brief Update UI with current playback position
     * @param position Current position in seconds
     * @param duration Total duration in seconds
     */
    void updatePlaybackPosition(double position, double duration);
    
private:
    /**
     * @brief Set up the menu bar with all menus
     */
    void setupMenuBar();
    
    /**
     * @brief Create the main toolbar with playback controls
     */
    void setupToolBar();
    
    /**
     * @brief Set up the central widget layout
     */
    void setupCentralWidget();
    
    /**
     * @brief Create audio control widgets
     */
    void createAudioControls();
    
    /**
     * @brief Create preset control widgets
     */
    void createPresetControls();
    
    // UI Elements
    ProjectMWidget* m_visualizer;
    PlaylistWidget* m_playlist;
    
    // Audio controls
    QPushButton* m_playPauseBtn;
    QPushButton* m_stopBtn;
    QPushButton* m_prevBtn;
    QPushButton* m_nextBtn;
    QSlider* m_seekSlider;
    QSlider* m_volumeSlider;
    QLabel* m_timeLabel;
    
    // Preset controls
    QPushButton* m_prevPresetBtn;
    QPushButton* m_nextPresetBtn;
    QPushButton* m_favoriteBtn;
    QPushButton* m_blacklistBtn;
    QLabel* m_presetNameLabel;
    
    // State
    bool m_isPlaying;
    
    // Actions
    QAction* m_addFilesAction;
    QAction* m_settingsAction;
    QAction* m_fullscreenAction;
    QAction* m_aboutAction;
    QAction* m_quitAction;
};

} // namespace NeonWave::GUI