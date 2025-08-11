#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace NeonWave::Visualizer {

class PresetManager {
public:
    static PresetManager& instance();

    bool isFavorite(const std::string& presetName) const;
    bool isBlacklisted(const std::string& presetName) const;

    void toggleFavorite(const std::string& presetName);
    void addToBlacklist(const std::string& presetName);

    std::vector<std::string> favorites() const;
    std::vector<std::string> blacklist() const;

    void load();
    void save() const;

private:
    PresetManager();

    std::unordered_set<std::string> m_favorites;
    std::unordered_set<std::string> m_blacklist;
};

} // namespace NeonWave::Visualizer
