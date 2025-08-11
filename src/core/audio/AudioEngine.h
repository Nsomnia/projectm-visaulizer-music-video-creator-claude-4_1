#pragma once

#include <QObject>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QStringList>
#include <memory>

namespace NeonWave::Core::Audio {

class AudioEngine : public QObject {
    Q_OBJECT
public:
    explicit AudioEngine(QObject* parent = nullptr);
    ~AudioEngine();

    void setPlaylist(const QStringList& files);
    void play();
    void pause();
    void stop();
    void next();
    void previous();

    // Connect ProjectM audio sink: emit pcm buffers to a callback
    using PCMCallback = std::function<void(const float* data, size_t samples)>;
    void setPCMCallback(PCMCallback cb);

signals:
    void positionChanged(qint64 positionMs, qint64 durationMs);
    void stateChanged(QMediaPlayer::PlaybackState state);

private slots:
    void onPositionChanged(qint64);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus);

private:
    void loadCurrent();

    std::unique_ptr<QMediaPlayer> m_player;
    std::unique_ptr<QAudioOutput> m_output;
    QStringList m_files;
    int m_index{ -1 };
    PCMCallback m_pcm;
};

} // namespace NeonWave::Core::Audio
