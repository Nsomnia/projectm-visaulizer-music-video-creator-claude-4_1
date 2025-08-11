#pragma once

#include <QObject>
#include <QMediaPlayer>
#include <QAudioBufferOutput>
#include <QStringList>
#include <memory>
#include <functional>

class QAudioBuffer;

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

    using PCMCallback = std::function<void(const float* data, size_t samples)>;
    void setPCMCallback(PCMCallback cb);

signals:
    void positionChanged(qint64 positionMs, qint64 durationMs);
    void stateChanged(QMediaPlayer::PlaybackState state);

private slots:
    void onPositionChanged(qint64);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus);
    void onAudioBufferReceived(const QAudioBuffer& buffer);

private:
    void loadCurrent();

    std::unique_ptr<QMediaPlayer> m_player;
    std::unique_ptr<QAudioBufferOutput> m_audio_output;
    QStringList m_files;
    int m_index{ -1 };
    PCMCallback m_pcm;
};

}
