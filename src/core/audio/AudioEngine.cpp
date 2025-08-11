/**
 * @file AudioEngine.cpp
 */

#include "AudioEngine.h"
#include <QAudioFormat>
#include <QIODevice>
#include <QBuffer>

namespace NeonWave::Core::Audio {

AudioEngine::AudioEngine(QObject* parent)
    : QObject(parent)
    , m_player(std::make_unique<QMediaPlayer>())
    , m_output(std::make_unique<QAudioOutput>()) {
    m_player->setAudioOutput(m_output.get());

    connect(m_player.get(), &QMediaPlayer::positionChanged,
            this, &AudioEngine::onPositionChanged);
    connect(m_player.get(), &QMediaPlayer::mediaStatusChanged,
            this, &AudioEngine::onMediaStatusChanged);
    connect(m_player.get(), &QMediaPlayer::playbackStateChanged,
            this, &AudioEngine::stateChanged);
}

AudioEngine::~AudioEngine() = default;

void AudioEngine::setPlaylist(const QStringList& files) {
    m_files = files;
    m_index = m_files.isEmpty() ? -1 : 0;
    loadCurrent();
}

void AudioEngine::play() {
    m_player->play();
}

void AudioEngine::pause() {
    m_player->pause();
}

void AudioEngine::stop() {
    m_player->stop();
}

void AudioEngine::next() {
    if (m_files.isEmpty()) return;
    m_index = (m_index + 1) % m_files.size();
    loadCurrent();
    play();
}

void AudioEngine::previous() {
    if (m_files.isEmpty()) return;
    m_index = (m_index - 1 + m_files.size()) % m_files.size();
    loadCurrent();
    play();
}

void AudioEngine::setPCMCallback(PCMCallback cb) { m_pcm = std::move(cb); }

void AudioEngine::onPositionChanged(qint64 pos) {
    emit positionChanged(pos, m_player->duration());
}

void AudioEngine::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        next();
    }
}

void AudioEngine::loadCurrent() {
    if (m_index < 0 || m_index >= m_files.size()) return;
    m_player->setSource(QUrl::fromLocalFile(m_files[m_index]));
}

} // namespace NeonWave::Core::Audio
