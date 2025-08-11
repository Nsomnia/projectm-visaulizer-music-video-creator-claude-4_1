    /**
 * @file AudioPlaylistWidget.cpp
 * @brief Implementation of audio playlist management widget
 */

#include "PlaylistWidget.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QFileInfo>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QUrl>

namespace NeonWave::GUI {

AudioPlaylistWidget::AudioPlaylistWidget(QWidget* parent)
    : QWidget(parent)
    , m_currentTrackIndex(-1) {
    setupUI();
}

AudioPlaylistWidget::~AudioPlaylistWidget() = default;

void AudioPlaylistWidget::setupUI() {
    auto* layout = new QVBoxLayout(this);
    
    // Header
    auto* header = new QLabel("Audio Playlist");
    header->setStyleSheet("QLabel { font-weight: bold; padding: 5px; }");
    layout->addWidget(header);
    
    // List widget
    m_listWidget = new QListWidget();
    m_listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_listWidget->setDefaultDropAction(Qt::MoveAction);
    m_listWidget->setAlternatingRowColors(true);
    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // Enable drag and drop from external sources
    setAcceptDrops(true);
    
    // Connect signals
    connect(m_listWidget, &QListWidget::itemDoubleClicked,
            this, &AudioPlaylistWidget::onItemDoubleClicked);
    
    connect(m_listWidget, &QListWidget::customContextMenuRequested,
            this, &AudioPlaylistWidget::showContextMenu);
    
    layout->addWidget(m_listWidget);
}

void AudioPlaylistWidget::addFiles(const QStringList& files) {
    for (const QString& file : files) {
        if (QFileInfo::exists(file)) {
            QString trackName = extractTrackName(file);
            auto* item = new QListWidgetItem(trackName);
            item->setData(Qt::UserRole, file); // Store full path
            item->setToolTip(file);
            m_listWidget->addItem(item);
        }
    }
    
    emit audioPlaylistChanged();
}

void AudioPlaylistWidget::clearPlaylist() {
    m_listWidget->clear();
    m_currentTrackIndex = -1;
    emit audioPlaylistChanged();
}

int AudioPlaylistWidget::getCurrentTrackIndex() const {
    return m_currentTrackIndex;
}

void AudioPlaylistWidget::setCurrentTrackIndex(int index) {
    // Clear previous highlighting
    for (int i = 0; i < m_listWidget->count(); ++i) {
        auto* item = m_listWidget->item(i);
        QFont font = item->font();
        font.setBold(false);
        item->setFont(font);
        item->setBackground(QBrush());
    }
    
    // Highlight current track
    if (index >= 0 && index < m_listWidget->count()) {
        auto* item = m_listWidget->item(index);
        QFont font = item->font();
        font.setBold(true);
        item->setFont(font);
        item->setBackground(QBrush(QColor(60, 60, 100)));
        m_currentTrackIndex = index;
    }
}

int AudioPlaylistWidget::getTrackCount() const {
    return m_listWidget->count();
}

QString AudioPlaylistWidget::getTrackPath(int index) const {
    if (index >= 0 && index < m_listWidget->count()) {
        return m_listWidget->item(index)->data(Qt::UserRole).toString();
    }
    return QString();
}

void AudioPlaylistWidget::onItemDoubleClicked(QListWidgetItem* item) {
    int index = m_listWidget->row(item);
    setCurrentTrackIndex(index);
    emit trackActivated(index);
}

void AudioPlaylistWidget::showContextMenu(const QPoint& pos) {
    QMenu menu(this);
    
    QAction* removeAction = menu.addAction("Remove");
    connect(removeAction, &QAction::triggered,
            this, &AudioPlaylistWidget::removeSelectedTracks);
    
    menu.addSeparator();
    
    QAction* clearAction = menu.addAction("Clear All");
    connect(clearAction, &QAction::triggered,
            this, &AudioPlaylistWidget::clearPlaylist);
    
    menu.exec(m_listWidget->mapToGlobal(pos));
}

void AudioPlaylistWidget::removeSelectedTracks() {
    auto selectedItems = m_listWidget->selectedItems();
    for (auto* item : selectedItems) {
        delete item;
    }
    emit audioPlaylistChanged();
}

void AudioPlaylistWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void AudioPlaylistWidget::dropEvent(QDropEvent* event) {
    QStringList files;
    for (const QUrl& url : event->mimeData()->urls()) {
        if (url.isLocalFile()) {
            files.append(url.toLocalFile());
        }
    }
    
    if (!files.isEmpty()) {
        addFiles(files);
        event->acceptProposedAction();
    }
}

QString AudioPlaylistWidget::extractTrackName(const QString& filePath) const {
    QFileInfo info(filePath);
    return info.fileName();
}

} // namespace NeonWave::GUI