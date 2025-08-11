# Video Recording System

## Recording Workflow

### Start Recording
1. Create temporary file in `/tmp/vibestream/`
2. Lock to favorite presets if any exist
3. Display recording overlay
4. Begin frame capture

### During Recording
1. Capture visualizer frames
2. Render text overlays
3. Encode to temporary file
4. Update time counter

### Stop Recording
1. Finalize video encoding
2. Generate filename from metadata
3. Move from `/tmp` to `$HOME/Videos/VibeStream/`
4. Unlock preset restrictions

## Filename Generation

### With ID3 Tags
`[Artist] - [Title]_[YYYY-MM-DD_HH-MM-SS].mp4`

### Without ID3 Tags
`[Filename]_[YYYY-MM-DD_HH-MM-SS].mp4`

## Encoding Settings

### Default Profile
- Codec: H.264
- Resolution: 1280x720 (16:9)
- Framerate: 30 fps
- Bitrate: 5 Mbps
- Audio: AAC 128 kbps

### Customizable Options
- Resolution presets (480p, 720p, 1080p)
- Framerate selection (30, 60 fps)
- Bitrate adjustment
- Codec selection (H.264, H.265)

## Temporary Storage

### Location
- `/tmp/vibestream/`
- Automatic cleanup on startup
- Session-based naming

### Interruption Handling
- Delete incomplete recordings
- Preserve completed recordings
- Log interruption events