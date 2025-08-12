#include "VideoExporter.h"
#include <iostream>

VideoExporter::VideoExporter() = default;

VideoExporter::~VideoExporter() {
    stop();
}

bool VideoExporter::start(int width, int height, int frameRate, const std::string& outputPath) {
    if (m_isRecording) {
        return false;
    }

    QString command = "ffmpeg";
    QStringList args;
    args << "-y"
         << "-f" << "rawvideo"
         << "-vcodec" << "rawvideo"
         << "-pix_fmt" << "rgb24"
         << "-s" << QString("%1x%2").arg(width).arg(height)
         << "-r" << QString::number(frameRate)
         << "-i" << "-"
         << "-c:v" << "libx264"
         << "-preset" << "ultrafast"
         << "-pix_fmt" << "yuv420p"
         << QString::fromStdString(outputPath);

    m_ffmpegProcess.start(command, args);

    if (!m_ffmpegProcess.waitForStarted()) {
        std::cerr << "Failed to start ffmpeg process" << std::endl;
        return false;
    }

    m_isRecording = true;
    return true;
}

void VideoExporter::stop() {
    if (!m_isRecording) {
        return;
    }

    m_ffmpegProcess.closeWriteChannel();
    m_ffmpegProcess.waitForFinished();
    m_isRecording = false;
}

void VideoExporter::writeFrame(const std::vector<unsigned char>& frameBuffer) {
    if (!m_isRecording) {
        return;
    }

    m_ffmpegProcess.write(reinterpret_cast<const char*>(frameBuffer.data()), frameBuffer.size());
}
