# Settings Management

## Configuration Files

### Main Configuration
Location: `$HOME/.config/vibestream/config.ini`

### Preset Lists
- Favorites: `$HOME/.config/vibestream/favorites.txt`
- Ignored: `$HOME/.config/vibestream/ignored.txt`

### Playlist
Location: `$HOME/.config/vibestream/playlist.m3u`

## Configuration Sections

### [General]
- `lastDirectory` - Last opened directory
- `volume` - Playback volume (0.0-1.0)
- `shuffle` - Shuffle mode enabled
- `repeat` - Repeat mode enabled

### [Visualizer]
- `fps` - Target frames per second
- `meshWidth` - Preset mesh width
- `meshHeight` - Preset mesh height
- `fullscreen` - Fullscreen mode

### [TextOverlay]
- `titleFont` - Title font family
- `titleSize` - Title font size
- `artistFont` - Artist font family
- `artistSize` - Artist font size
- `effectsEnabled` - Global effects toggle
- `transitionDuration` - Fade duration (ms)

### [Recording]
- `resolution` - Output resolution
- `framerate` - Output framerate
- `bitrate` - Output bitrate (kbps)
- `codec` - Output codec

### [Paths]
- `presetDirectory` - MilkDrop preset directory
- `videoOutput` - Video output directory
- `tempDirectory` - Temporary file directory

## Default Values

All settings have sensible defaults and will be created on first run if not present.