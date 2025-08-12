#include "Config.h"
#include "Application.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <fstream>

namespace NeonWave::Core {

Config& Config::instance() {
    static Config cfg;
    return cfg;
}

std::filesystem::path Config::settingsFilePath() const {
    return Application::instance().getConfigPath() / "settings.json";
}

std::filesystem::path Config::favoritesFilePath() const {
    return Application::instance().getConfigPath() / "favorites.json";
}

std::filesystem::path Config::blacklistFilePath() const {
    return Application::instance().getConfigPath() / "blacklist.json";
}

void Config::load() {
    const auto path = settingsFilePath();
    QFile file(QString::fromStdString(path.string()));
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly)) return;

    const auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) return;
    const auto root = doc.object();

    // Audio
    if (root.contains("audio")) {
        const auto a = root.value("audio").toObject();
        if (a.contains("volume")) m_audio.volume = a.value("volume").toDouble(0.7);
    }

    // Visualizer
    if (root.contains("visualizer")) {
        const auto v = root.value("visualizer").toObject();
        if (v.contains("fps")) m_visualizer.fps = v.value("fps").toInt(60);
        if (v.contains("mesh_x")) m_visualizer.meshX = v.value("mesh_x").toInt(32);
        if (v.contains("mesh_y")) m_visualizer.meshY = v.value("mesh_y").toInt(24);
        if (v.contains("aspect_correction")) m_visualizer.aspectCorrection = v.value("aspect_correction").toBool(true);
        if (v.contains("beat_sensitivity")) m_visualizer.beatSensitivity = static_cast<float>(v.value("beat_sensitivity").toDouble(1.0));
        if (v.contains("hard_cut_enabled")) m_visualizer.hardCutEnabled = v.value("hard_cut_enabled").toBool(true);
        if (v.contains("hard_cut_duration")) m_visualizer.hardCutDuration = v.value("hard_cut_duration").toDouble(3.0);
        if (v.contains("soft_cut_duration")) m_visualizer.softCutDuration = v.value("soft_cut_duration").toDouble(10.0);
        if (v.contains("preset_duration")) m_visualizer.presetDuration = v.value("preset_duration").toDouble(30.0);
        if (v.contains("preset_locked")) m_visualizer.presetLocked = v.value("preset_locked").toBool(false);
        if (v.contains("preset_directory")) m_visualizer.presetDirectory = v.value("preset_directory").toString().toStdString();
        if (v.contains("texture_directory")) m_visualizer.textureDirectory = v.value("texture_directory").toString().toStdString();
        if (v.contains("debug_inject_test_signal")) m_visualizer.debugInjectTestSignal = v.value("debug_inject_test_signal").toBool(false);
        if (v.contains("load_random_on_startup")) m_visualizer.loadRandomPresetOnStartup = v.value("load_random_on_startup").toBool(false);
    }
}

void Config::save() const {
    QJsonObject root;

    // Audio
    QJsonObject a;
    a.insert("volume", m_audio.volume);
    root.insert("audio", a);

    // Visualizer
    QJsonObject v;
    v.insert("fps", m_visualizer.fps);
    v.insert("mesh_x", m_visualizer.meshX);
    v.insert("mesh_y", m_visualizer.meshY);
    v.insert("aspect_correction", m_visualizer.aspectCorrection);
    v.insert("beat_sensitivity", m_visualizer.beatSensitivity);
    v.insert("hard_cut_enabled", m_visualizer.hardCutEnabled);
    v.insert("hard_cut_duration", m_visualizer.hardCutDuration);
    v.insert("soft_cut_duration", m_visualizer.softCutDuration);
    v.insert("preset_duration", m_visualizer.presetDuration);
    v.insert("preset_locked", m_visualizer.presetLocked);
    v.insert("preset_directory", QString::fromStdString(m_visualizer.presetDirectory));
    v.insert("texture_directory", QString::fromStdString(m_visualizer.textureDirectory));
    v.insert("debug_inject_test_signal", m_visualizer.debugInjectTestSignal);
    v.insert("load_random_on_startup", m_visualizer.loadRandomPresetOnStartup);
    root.insert("visualizer", v);

    const auto path = settingsFilePath();
    QFile file(QString::fromStdString(path.string()));
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    }
}

VisualizerConfig& Config::visualizer() { return m_visualizer; }
const VisualizerConfig& Config::visualizer() const { return m_visualizer; }

AudioConfig& Config::audio() { return m_audio; }
const AudioConfig& Config::audio() const { return m_audio; }

} // namespace NeonWave::Core
