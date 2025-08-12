#pragma once

#include <string>
#include <vector>
#include <QProcess>

class VideoExporter {
public:
    VideoExporter();
    ~VideoExporter();

    bool start(int width, int height, int frameRate, const std::string& outputPath);
    void stop();
    void writeFrame(const std::vector<unsigned char>& frameBuffer);

    bool isRecording() const { return m_isRecording; }

private:
    QProcess m_ffmpegProcess;
    bool m_isRecording = false;
};
