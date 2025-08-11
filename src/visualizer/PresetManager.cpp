#include "PresetManager.h"
#include "core/Config.h"
#include "core/Application.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>

#include <algorithm>

namespace NeonWave::Visualizer {

PresetManager& PresetManager::instance() {
    static PresetManager mgr;
    return mgr;
}

PresetManager::PresetManager() {
    load();
}

static std::unordered_set<std::string> readStringSetFromJsonFile(const std::filesystem::path& path) {
    std::unordered_set<std::string> out;
    QFile file(QString::fromStdString(path.string()));
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) return out;
    const auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) return out;
    for (const auto& v : doc.array()) {
        out.insert(v.toString().toStdString());
    }
    return out;
}

static void writeStringSetToJsonFile(const std::filesystem::path& path, const std::unordered_set<std::string>& set) {
    QJsonArray arr;
    for (const auto& s : set) arr.push_back(QString::fromStdString(s));
    QFile file(QString::fromStdString(path.string()));
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
        file.close();
    }
}

void PresetManager::load() {
    const auto fav = Core::Config::instance().favoritesFilePath();
    const auto bl = Core::Config::instance().blacklistFilePath();
    m_favorites = readStringSetFromJsonFile(fav);
    m_blacklist = readStringSetFromJsonFile(bl);
}

void PresetManager::save() const {
    const auto fav = Core::Config::instance().favoritesFilePath();
    const auto bl = Core::Config::instance().blacklistFilePath();
    writeStringSetToJsonFile(fav, m_favorites);
    writeStringSetToJsonFile(bl, m_blacklist);
}

bool PresetManager::isFavorite(const std::string& presetName) const {
    return m_favorites.count(presetName) > 0;
}

bool PresetManager::isBlacklisted(const std::string& presetName) const {
    return m_blacklist.count(presetName) > 0;
}

void PresetManager::toggleFavorite(const std::string& presetName) {
    if (isFavorite(presetName)) m_favorites.erase(presetName);
    else m_favorites.insert(presetName);
    save();
}

void PresetManager::addToBlacklist(const std::string& presetName) {
    m_blacklist.insert(presetName);
    save();
}

std::vector<std::string> PresetManager::favorites() const {
    return std::vector<std::string>(m_favorites.begin(), m_favorites.end());
}

std::vector<std::string> PresetManager::blacklist() const {
    return std::vector<std::string>(m_blacklist.begin(), m_blacklist.end());
}

} // namespace NeonWave::Visualizer
