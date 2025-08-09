/**
 * @file PlaylistWidget.h
 * @brief Playlist management widget
 */

#pragma once

#include <QWidget>
#include <QStringList>
#include <memory>

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
QT_END_NAMESPACE

namespace NeonWave::GUI {

/**
 * @class PlaylistWidget
 * @brief Widget for managing audio file playlist
 * 
 * Features:
 * - Drag and drop support
 * - Reorderable items
 * - Current track highlighting
 * - Context menu for item operations
 */
class PlaylistWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit PlaylistWidget(QWidget* parent = nullptr);
    ~PlaylistWidget();
    
    /**
     * @brief Add files to playlist
     * @param files List of file paths
     */
    void addFiles(const QStringList& files);
    
    /**
     * @brief Clear all items from playlist
     */
    void clearPlaylist();
    
    /**
     * @brief Get current track index
     * @return Index of current track, -1 if none
     */
    int getCurrentTrackIndex() const;
    
    /**
     * @brief Set current track by index
     * @param index Track index
     */
    void setCurrentTrackIndex(int index);
    
    /**
     * @brief Get total number of tracks
     * @return Track count
     */
    int getTrackCount() const;
    
    /**
     * @brief Get file path at index
     * @param index Track index
     * @return File path or empty string
     */
    QString getTrackPath(int index) const;
    
signals:
    /**
     * @brief Emitted when user double-clicks a track
     * @param index Track index
     */
    void trackActivated(int index);
    
    /**
     * @brief Emitted when playlist changes
     */
    void playlistChanged();
    
private slots:
    /**
     * @brief Handle item double-click
     * @param item Clicked item
     */
    void onItemDoubleClicked(QListWidgetItem* item);
    
    /**
     * @brief Show context menu
     * @param pos Mouse position
     */
    void showContextMenu(const QPoint& pos);
    
    /**
     * @brief Remove selected tracks
     */
    void removeSelectedTracks();
    
protected:
    /**
     * @brief Handle drag enter events
     */
    void dragEnterEvent(QDragEnterEvent* event) override;
    
    /**
     * @brief Handle drop events
     */
    void dropEvent(QDropEvent* event) override;
    
private:
    /**
     * @brief Set up the widget UI
     */
    void setupUI();
    
    /**
     * @brief Extract track name from path
     * @param filePath Full file path
     * @return Display name for track
     */
    QString extractTrackName(const QString& filePath) const;
    
    QListWidget* m_listWidget;
    int m_currentTrackIndex;
};

} // namespace NeonWave::GUI