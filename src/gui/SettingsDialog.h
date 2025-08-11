/**
 * @file SettingsDialog.h
 * @brief Application settings dialog
 */

#pragma once

#include <QDialog>
#include <memory>

QT_BEGIN_NAMESPACE
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QTabWidget;
QT_END_NAMESPACE

namespace NeonWave::GUI {

class SettingsDialog : public QDialog {

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();

private:
    void buildUI();
    void loadFromConfig();
    void saveToConfig();

    // Visualizer tab controls
    QSpinBox* m_fps{};
    QSpinBox* m_meshX{};
    QSpinBox* m_meshY{};
    QCheckBox* m_aspect{};
    QDoubleSpinBox* m_beat{};
    QCheckBox* m_hardCutEnabled{};
    QDoubleSpinBox* m_hardCut{};
    QDoubleSpinBox* m_softCut{};
    QDoubleSpinBox* m_presetDuration{};
    QCheckBox* m_presetLocked{};
    QLineEdit* m_presetDir{};
    QPushButton* m_browsePresetDir{};
    QLineEdit* m_textureDir{};
    QPushButton* m_browseTextureDir{};
};

} // namespace NeonWave::GUI
