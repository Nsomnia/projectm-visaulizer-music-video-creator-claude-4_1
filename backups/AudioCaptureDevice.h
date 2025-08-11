#pragma once

#include <QIODevice>
#include <functional>

namespace NeonWave::Core::Audio {

class AudioCaptureDevice : public QIODevice {
    Q_OBJECT
public:
    using PCMCallback = std::function<void(const char* data, qint64 size)>;

    explicit AudioCaptureDevice(QObject* parent = nullptr);
    void setPCMCallback(PCMCallback cb);

protected:
    qint64 readData(char* data, qint64 maxSize) override;
    qint64 writeData(const char* data, qint64 maxSize) override;

private:
    PCMCallback m_pcm;
};

}
