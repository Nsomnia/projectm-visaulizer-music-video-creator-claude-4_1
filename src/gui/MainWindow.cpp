/**
 * @file MainWindow.cpp
 * @brief Implementation of the main application window
 */

#include "MainWindow.h"
#include "visualizer/ProjectMWidget.h"
#include "PlaylistWidget.h"
#include "SettingsDialog.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QStyle>
#include <QTimer>

namespace NeonWave::GUI {

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent)
    , m_isPlaying(false) {
    
    // Set window properties
    setWindowTitle("NeonWave - Audio Visualizer");
    resize(1280, 720);
    
    // Initialize UI
    setupMenuBar();
    setupToolBar();
    setupCentralWidget();
    
    // Set up status bar
    statusBar()->showMessage("Ready");
    
    // Connect signals for internal state management
    connect(this, &MainWindow::playStateChanged, 
            [this](bool playing) {
                m_isPlaying = playing;
                m_playPauseBtn->setIcon(style()->standardIcon(
                    playing ? QStyle::SP_MediaPause : QStyle::SP_MediaPlay));
            });
}

MainWindow::~MainWindow() = default;

void MainWindow::setupMenuBar() {
    auto* menuBar = this->menuBar();
    
    // File menu
    auto* fileMenu = menuBar->addMenu("&File");
    
    m_addFilesAction = fileMenu->addAction("&Add Files...");
    m_addFilesAction->setShortcut(QKeySequence::Open);
    connect(m_addFilesAction, &QAction::triggered, 
            this, &MainWindow::onAddFilesClicked);
    
    fileMenu->addSeparator();
    
    m_quitAction = fileMenu->addAction("&Quit");
    m_quitAction->setShortcut(QKeySequence::Quit);
    connect(m_quitAction, &QAction::triggered, this, &QWidget::close);
    
    // View menu
    auto* viewMenu = menuBar->addMenu("&View");
    
    m_fullscreenAction = viewMenu->addAction("&Fullscreen");
    m_fullscreenAction->setShortcut(Qt::Key_F11);
    m_fullscreenAction->setCheckable(true);
    connect(m_fullscreenAction, &QAction::triggered, [this](bool checked) {
        if (checked) {
            showFullScreen();
        } else {
            showNormal();
        }
    });
    
    // Settings menu
    auto* settingsMenu = menuBar->addMenu("&Settings");
    
    m_settingsAction = settingsMenu->addAction("&Preferences...");
    m_settingsAction->setShortcut(QKeySequence::Preferences);
    connect(m_settingsAction, &QAction::triggered, 
            this, &MainWindow::onSettingsClicked);
    
    // Help menu
    auto* helpMenu = menuBar->addMenu("&Help");
    
    m_aboutAction = helpMenu->addAction("&About NeonWave");
    connect(m_aboutAction, &QAction::triggered, 
            this, &MainWindow::onAboutClicked);
}

void MainWindow::setupToolBar() {
    auto* toolbar = addToolBar("Main Controls");
    toolbar->setMovable(false);
    
    // Add file button
    toolbar->addAction(style()->standardIcon(QStyle::SP_FileDialogStart),
                       "Add Files", this, &MainWindow::onAddFilesClicked);
    
    toolbar->addSeparator();
    
    // Create audio controls
    createAudioControls();
    
    // Add audio controls to toolbar
    toolbar->addWidget(m_prevBtn);
    toolbar->addWidget(m_playPauseBtn);
    toolbar->addWidget(m_stopBtn);
    toolbar->addWidget(m_nextBtn);
    
    toolbar->addSeparator();
    
    // Time display and seek slider
    toolbar->addWidget(m_timeLabel);
    toolbar->addWidget(m_seekSlider);
    
    toolbar->addSeparator();
    
    // Volume control
    toolbar->addWidget(new QLabel("Volume:"));
    toolbar->addWidget(m_volumeSlider);
}

void MainWindow::setupCentralWidget() {
    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto* layout = new QHBoxLayout(centralWidget);
    
    // Create splitter for resizable panels
    auto* splitter = new QSplitter(Qt::Horizontal);
    
    // Left panel - Visualizer and preset controls
    auto* leftPanel = new QWidget();
    auto* leftLayout = new QVBoxLayout(leftPanel);
    
    // ProjectM visualizer widget
    m_visualizer = new ProjectMWidget(this);
    leftLayout->addWidget(m_visualizer, 1); // Stretch factor 1
    
    // Preset control bar
    auto* presetBar = new QWidget();
    auto* presetLayout = new QHBoxLayout(presetBar);
    presetLayout->setContentsMargins(5, 5, 5, 5);
    
    createPresetControls();
    
    presetLayout->addWidget(new QLabel("Preset:"));
    presetLayout->addWidget(m_presetNameLabel, 1);
    presetLayout->addWidget(m_prevPresetBtn);
    presetLayout->addWidget(m_nextPresetBtn);
    presetLayout->addWidget(m_favoriteBtn);
    presetLayout->addWidget(m_blacklistBtn);
    
    leftLayout->addWidget(presetBar);
    
    // Right panel - Playlist
    m_playlist = new PlaylistWidget(this);
    
    // Add panels to splitter
    splitter->addWidget(leftPanel);
    splitter->addWidget(m_playlist);
    splitter->setStretchFactor(0, 3); // Visualizer gets 3/4 of space
    splitter->setStretchFactor(1, 1); // Playlist gets 1/4
    
    layout->addWidget(splitter);
}

void MainWindow::createAudioControls() {
    // Play/Pause button
    m_playPauseBtn = new QPushButton();
    m_playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_playPauseBtn->setToolTip("Play/Pause");
    connect(m_playPauseBtn, &QPushButton::clicked, 
            this, &MainWindow::onPlayPauseToggled);
    
    // Stop button
    m_stopBtn = new QPushButton();
    m_stopBtn->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    m_stopBtn->setToolTip("Stop");
    connect(m_stopBtn, &QPushButton::clicked, 
            this, &MainWindow::onStopClicked);
    
    // Previous button
    m_prevBtn = new QPushButton();
    m_prevBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    m_prevBtn->setToolTip("Previous Track");
    connect(m_prevBtn, &QPushButton::clicked, 
            this, &MainWindow::onPreviousTrackClicked);
    
    // Next button
    m_nextBtn = new QPushButton();
    m_nextBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    m_nextBtn->setToolTip("Next Track");
    connect(m_nextBtn, &QPushButton::clicked, 
            this, &MainWindow::onNextTrackClicked);
    
    // Time label
    m_timeLabel = new QLabel("00:00 / 00:00");
    m_timeLabel->setMinimumWidth(100);
    
    // Seek slider
    m_seekSlider = new QSlider(Qt::Horizontal);
    m_seekSlider->setMinimumWidth(200);
    connect(m_seekSlider, &QSlider::sliderMoved, [this](int value) {
        double position = static_cast<double>(value) / 1000.0;
        emit seekRequested(position);
    });
    
    // Volume slider
    m_volumeSlider = new QSlider(Qt::Horizontal);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(70);
    m_volumeSlider->setMaximumWidth(100);
    m_volumeSlider->setToolTip("Volume");
    connect(m_volumeSlider, &QSlider::valueChanged, [this](int value) {
        emit volumeChanged(static_cast<double>(value) / 100.0);
    });
}

void MainWindow::createPresetControls() {
    // Preset name label
    m_presetNameLabel = new QLabel("No preset loaded");
    m_presetNameLabel->setStyleSheet("QLabel { font-weight: bold; }");
    
    // Previous preset
    m_prevPresetBtn = new QPushButton();
    m_prevPresetBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    m_prevPresetBtn->setToolTip("Previous Preset");
    connect(m_prevPresetBtn, &QPushButton::clicked, [this]() {
        if (m_visualizer) m_visualizer->previousPreset();
    });

    // Next preset
    m_nextPresetBtn = new QPushButton();
    m_nextPresetBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    m_nextPresetBtn->setToolTip("Next Preset");
    connect(m_nextPresetBtn, &QPushButton::clicked, [this]() {
        if (m_visualizer) m_visualizer->nextPreset();
    });

    // Favorite button
    m_favoriteBtn = new QPushButton();
    m_favoriteBtn->setIcon(style()->standardIcon(QStyle::SP_DialogYesButton));
    m_favoriteBtn->setToolTip("Toggle Favorite");
    m_favoriteBtn->setCheckable(true);
    connect(m_favoriteBtn, &QPushButton::clicked, 
            this, &MainWindow::onToggleFavoritePreset);
    
    // Blacklist button
    m_blacklistBtn = new QPushButton();
    m_blacklistBtn->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    m_blacklistBtn->setToolTip("Blacklist Preset");
    connect(m_blacklistBtn, &QPushButton::clicked, 
            this, &MainWindow::onBlacklistPreset);

    // Update preset label on changes from visualizer
    if (m_visualizer) {
        connect(m_visualizer, &ProjectMWidget::presetChanged, this, [this](const QString& name) {
            m_presetNameLabel->setText(name);
        });
    }
}

void MainWindow::onPlayPauseToggled() {
    m_isPlaying = !m_isPlaying;
    emit playStateChanged(m_isPlaying);
    statusBar()->showMessage(m_isPlaying ? "Playing" : "Paused");
}

void MainWindow::onStopClicked() {
    m_isPlaying = false;
    emit playStateChanged(false);
    m_seekSlider->setValue(0);
    statusBar()->showMessage("Stopped");
}

void MainWindow::onNextTrackClicked() {
    // Emit signal for audio engine to handle
    statusBar()->showMessage("Next track");
}

void MainWindow::onPreviousTrackClicked() {
    // Emit signal for audio engine to handle
    statusBar()->showMessage("Previous track");
}

void MainWindow::onAddFilesClicked() {
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Select Audio Files",
        QDir::homePath(),
        "Audio Files (*.mp3 *.wav *.flac *.ogg *.m4a);;All Files (*.*)"
    );
    
    if (!files.isEmpty()) {
        // Add files to playlist
        m_playlist->addFiles(files);
        statusBar()->showMessage(QString("Added %1 files").arg(files.size()));
    }
}

void MainWindow::onSettingsClicked() {
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAboutClicked() {
    QMessageBox::about(this, "About NeonWave",
        "<h2>NeonWave 1.0.0</h2>"
        "<p>Advanced audio visualizer powered by ProjectM</p>"
        "<p>Built with Qt6 and modern C++20</p>"
        "<p>Designed for AI-assisted development</p>"
        "<p><a href='https://github.com/yourusername/neonwave'>GitHub Repository</a></p>"
    );
}

void MainWindow::onToggleFavoritePreset() {
    bool isFavorite = m_favoriteBtn->isChecked();
    QString message = isFavorite ? "Preset added to favorites" 
                                 : "Preset removed from favorites";
    statusBar()->showMessage(message);
    
    // TODO: Call PresetManager to save favorite state
}

void MainWindow::onBlacklistPreset() {
    int ret = QMessageBox::question(this, "Blacklist Preset",
        "Are you sure you want to blacklist this preset?\n"
        "It will never be shown again unless you remove it from the blacklist.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        statusBar()->showMessage("Preset blacklisted");
        // TODO: Call PresetManager to blacklist preset
    }
}

void MainWindow::updatePlaybackPosition(double position, double duration) {
    // Update seek slider
    if (!m_seekSlider->isSliderDown()) {
        m_seekSlider->setMaximum(static_cast<int>(duration * 1000));
        m_seekSlider->setValue(static_cast<int>(position * 1000));
    }
    
    // Update time label
    auto formatTime = [](double seconds) -> QString {
        int mins = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        return QString("%1:%2").arg(mins, 2, 10, QChar('0'))
                               .arg(secs, 2, 10, QChar('0'));
    };
    
    m_timeLabel->setText(QString("%1 / %2")
        .arg(formatTime(position))
        .arg(formatTime(duration)));
}

} // namespace NeonWave::GUI