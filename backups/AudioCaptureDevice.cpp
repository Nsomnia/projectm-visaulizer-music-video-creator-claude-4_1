#include "AudioCaptureDevice.h"

namespace NeonWave::Core::Audio {

AudioCaptureDevice::AudioCaptureDevice(QObject* parent) : QIODevice(parent) {
    open(QIODevice::WriteOnly);
}

void AudioCaptureDevice::setPCMCallback(PCMCallback cb) {
    m_pcm = std::move(cb);
}

qint64 AudioCaptureDevice::readData(char* data, qint64 maxSize) {
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return -1; // Not a readable device
}

qint64 AudioCaptureDevice::writeData(const char* data, qint64 maxSize) {
    if (m_pcm) {
        m_pcm(data, maxSize);
    }
    return maxSize;
}

}
