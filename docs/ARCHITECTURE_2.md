# Application Architecture

## Core Components

### AudioEngine
Handles audio playback and ID3 tag extraction
- Uses TagLib for metadata
- Manages playback state (play/pause/seek)
- Interfaces with system audio

### Visualizer
Manages ProjectM integration
- Loads and renders MilkDrop presets
- Handles preset navigation and history
- Manages favorites and ignored presets

### TextOverlay
Controls text rendering over visualizer
- Animated transitions (fade in/out)
- Dynamic effects (breathing, blooming, shimmer)
- Random positioning with border reflection

### PlaylistManager
Manages audio file playlist
- Loading/saving M3U playlists
- Rearranging tracks
- Playlist persistence

### VideoRecorder
Handles visualization recording
- FFmpeg integration
- Temporary file management (record to /tmp and only save final video if the track reaches n% of full playback which can be set in settings)
- Final video file directory placement
- Ensure file overwrite should not happen, and gracefully handle conflicts.

### SettingsManager
Handles configuration persistence
- Loading/saving user preferences (saved to a directory for the project located in $HOME/.config seems logical. Perhaps buttons to load and save from/to file to have various "modes" the user can easily change between)
- Default value management
- Configuration file I/O

## UI Components

### MainWindow
Main application window
- Visualizer viewport is the main and largest portion nof the main window user interface and should remain a static proportion percentage of the entire main window (still allow scaling based on full main window resizing by the end user)
- Playback and other main controls similar to a music player like Clementine or Strawberry, including a seek bar
- Playlist view
- Preset controls

### ProjectM settings
- Settings that can be viewed from projectM docs and source code with a good starting point and example of how the devs recccomend handling settings found in the projectm example sdl projects like https://github.com/projectM-visualizer/frontend-sdl-cpp and https://github.com/projectM-visualizer/frontend-sdl-rust
- All of the public API settings that are public should be avilable from https://raw.githubusercontent.com/projectM-visualizer/projectm/refs/heads/master/src/api/include/projectM-4/parameters.h as well as from all the files of said exposed public API that can without needing to directly linking or including as described in its readme at https://github.com/projectM-visualizer/projectm/tree/master/src/api
- These settings more often than not should be able to be kept to the settings window in their own tab/section as well as any other not changed frequently but we want *the maximum possible customization abiltiies*. 

### SettingsDialog
Comprehensive settings interface inside a separate window with a means of separating different categories of settings.
- Visualizer parameters
- Text overlay settings (User URL, Fallback Artist Name, static or animated, animation style, animation settings, color, shadow, border, transparency, inital transparency, fade out final transparency, delay tune before fade out startes, fade out transparency final value) (the text should do the exact inverse of the fade out effect at the inverse time from the end of the audio file playing)
- Recording options
- Preset management

### RecordingOverlay
Video recording indicators
- Recording status whether the record button is lit-up when receording is active or some part of the ui has a red border around it etc.
- Time counter
- Preset lock indicator (only use favorited presets during recording toggled on/off in settings)
- Must be able to have some means to record as many audio files to visualizer videos sequentally as the user wishes without needing any user interaction for cases when the user has hundreds or thousands of songs to create videos for i.e. be able to enable record an entire audio playlist to separate music videos one after the other without needing to do so programmatically with a cli application.