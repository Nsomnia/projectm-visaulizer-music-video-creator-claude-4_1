/**
 * @file SettingsDialog.cpp
 * @brief Basic settings dialog implementation (placeholder)
 */

#include "SettingsDialog.h"
#include "core/Config.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QTabWidget>

namespace NeonWave::GUI {

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Preferences");
    resize(520, 420);
    buildUI();
    loadFromConfig();
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::buildUI() {
    auto* outer = new QVBoxLayout(this);

    auto* tabs = new QTabWidget(this);
    outer->addWidget(tabs, 1);

    // Visualizer tab
    auto* visTab = new QWidget(this);
    auto* visForm = new QFormLayout(visTab);

    m_fps = new QSpinBox(visTab);
    m_fps->setRange(15, 240);
    visForm->addRow("FPS", m_fps);

    m_meshX = new QSpinBox(visTab);
    m_meshX->setRange(8, 128);
    m_meshY = new QSpinBox(visTab);
    m_meshY->setRange(8, 128);
    auto* meshRow = new QWidget(visTab);
    auto* meshLayout = new QHBoxLayout(meshRow);
    meshLayout->setContentsMargins(0, 0, 0, 0);
    meshLayout->addWidget(new QLabel("X:"));
    meshLayout->addWidget(m_meshX);
    meshLayout->addSpacing(12);
    meshLayout->addWidget(new QLabel("Y:"));
    meshLayout->addWidget(m_meshY);
    visForm->addRow("Mesh", meshRow);

    m_aspect = new QCheckBox("Enable aspect correction", visTab);
    visForm->addRow(m_aspect);

    m_beat = new QDoubleSpinBox(visTab);
    m_beat->setRange(0.1, 5.0);
    m_beat->setSingleStep(0.1);
    visForm->addRow("Beat sensitivity", m_beat);

    m_hardCutEnabled = new QCheckBox("Enable hard cut transitions", visTab);
    visForm->addRow(m_hardCutEnabled);

    m_hardCut = new QDoubleSpinBox(visTab);
    m_hardCut->setRange(0.0, 30.0);
    m_hardCut->setSingleStep(0.5);
    visForm->addRow("Hard cut duration (s)", m_hardCut);

    m_softCut = new QDoubleSpinBox(visTab);
    m_softCut->setRange(0.0, 120.0);
    m_softCut->setSingleStep(0.5);
    visForm->addRow("Soft cut duration (s)", m_softCut);

    m_presetDuration = new QDoubleSpinBox(visTab);
    m_presetDuration->setRange(1.0, 600.0);
    m_presetDuration->setSingleStep(1.0);
    visForm->addRow("Preset duration (s)", m_presetDuration);

    m_presetLocked = new QCheckBox("Lock current preset (no auto switch)", visTab);
    visForm->addRow(m_presetLocked);

    // Directories
    m_presetDir = new QLineEdit(visTab);
    m_browsePresetDir = new QPushButton("Browse...", visTab);
    auto* presetDirRow = new QWidget(visTab);
    auto* presetDirLayout = new QHBoxLayout(presetDirRow);
    presetDirLayout->setContentsMargins(0, 0, 0, 0);
    presetDirLayout->addWidget(m_presetDir, 1);
    presetDirLayout->addWidget(m_browsePresetDir);
    visForm->addRow("Preset directory", presetDirRow);
    connect(m_browsePresetDir, &QPushButton::clicked, this, [this]() {
        const auto dir = QFileDialog::getExistingDirectory(this, "Select preset directory");
        if (!dir.isEmpty()) m_presetDir->setText(dir);
    });

    m_textureDir = new QLineEdit(visTab);
    m_browseTextureDir = new QPushButton("Browse...", visTab);
    auto* textureDirRow = new QWidget(visTab);
    auto* textureDirLayout = new QHBoxLayout(textureDirRow);
    textureDirLayout->setContentsMargins(0, 0, 0, 0);
    textureDirLayout->addWidget(m_textureDir, 1);
    textureDirLayout->addWidget(m_browseTextureDir);
    visForm->addRow("Texture directory", textureDirRow);
    connect(m_browseTextureDir, &QPushButton::clicked, this, [this]() {
        const auto dir = QFileDialog::getExistingDirectory(this, "Select texture directory");
        if (!dir.isEmpty()) m_textureDir->setText(dir);
    });

    tabs->addTab(visTab, "Visualizer");

    // Buttons
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        saveToConfig();
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    outer->addWidget(buttons);
}

void SettingsDialog::loadFromConfig() {
    auto& cfg = NeonWave::Core::Config::instance();
    cfg.load();
    const auto& v = cfg.visualizer();
    m_fps->setValue(v.fps);
    m_meshX->setValue(v.meshX);
    m_meshY->setValue(v.meshY);
    m_aspect->setChecked(v.aspectCorrection);
    m_beat->setValue(v.beatSensitivity);
    m_hardCutEnabled->setChecked(v.hardCutEnabled);
    m_hardCut->setValue(v.hardCutDuration);
    m_softCut->setValue(v.softCutDuration);
    m_presetDuration->setValue(v.presetDuration);
    m_presetLocked->setChecked(v.presetLocked);
    m_presetDir->setText(QString::fromStdString(v.presetDirectory));
    m_textureDir->setText(QString::fromStdString(v.textureDirectory));
}

void SettingsDialog::saveToConfig() {
    auto& cfg = NeonWave::Core::Config::instance();
    auto& v = cfg.visualizer();
    v.fps = m_fps->value();
    v.meshX = m_meshX->value();
    v.meshY = m_meshY->value();
    v.aspectCorrection = m_aspect->isChecked();
    v.beatSensitivity = static_cast<float>(m_beat->value());
    v.hardCutEnabled = m_hardCutEnabled->isChecked();
    v.hardCutDuration = m_hardCut->value();
    v.softCutDuration = m_softCut->value();
    v.presetDuration = m_presetDuration->value();
    v.presetLocked = m_presetLocked->isChecked();
    v.presetDirectory = m_presetDir->text().toStdString();
    v.textureDirectory = m_textureDir->text().toStdString();
    cfg.save();
}

} // namespace NeonWave::GUI
