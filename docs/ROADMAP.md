# NeonWave Roadmap and Implementation Plan

This document captures the near-term plan, feature backlog, testing strategy, and key decisions for NeonWave. It is intended for both maintainers and contributors to track progress and align on priorities.

## Goals
- Provide a robust, modern GUI visualizer powered by libprojectM
- Deliver reliable audio visualization with a sensible defaults-first experience
- Support preset management (favorites, blacklist), hotkeys, and recording to video
- Keep the build easy (CMake, automatic libprojectM fetch or submodule)

## Current state (2025-08)
- Build system
  - CMake builds the app and libprojectM v4.x
  - Uses `external/projectm` if present; otherwise FetchContent clone (repo/tag configurable)
- Visualizer
  - Qt `ProjectMWidget` renders with OpenGL CompatibilityProfile
  - Default idle preset loaded on startup for easy debug visibility
  - Preset prev/next buttons, favorite toggle, blacklist button in UI
- Settings
  - Settings dialog with visualizer tweaks: FPS, mesh, aspect correction, beat sensitivity, hard/soft cut, preset duration, preset lock, preset/texture directories
  - JSON settings persisted under `~/.config/neonwave/settings.json`
- Presets
  - `PresetManager` persists favorites/blacklist in JSON under `~/.config/neonwave/`

## High-level architecture (reference)
- Core: `Application`, `Config`
- GUI: `MainWindow`, `ProjectMWidget`, `PlaylistWidget`, `SettingsDialog`
- Visualizer: libprojectM v4 API integration, preset playlist
- Future: `AudioEngine` to feed PCM to projectM and audio output to PipeWire/PulseAudio

## Roadmap by area

### 1) ProjectM integration
- v4 primary support (current)
- v3 compatibility mode (optional. NOT IMPORTANT UNTIL ALL FEATURES DONE)
  - CMake option `NEONWAVE_PROJECTM_API=v4|v3`
  - Adapter layer: `ProjectMAdapter` wrapper with unified interface
  - Only compile headers and calls for selected API version
- Parameter mapping
  - Ensure all adjustable parameters from docs are exposed in Settings
  - Implement UV/coordinate flip toggle when required by preset/drivers
- Preset loading
  - Apply blacklist when building playlist
  - Optional: recursive directory scan and import preset packs

### 2) Audio engine
- Input/decoding
  - Use ffmpeg or libsndfile+taglib for formats (mp3, flac, ogg, wav, m4a)
  - Decode to float stereo interleaved
- Output
  - PipeWire/PulseAudio via QtMultimedia or native client
- Feeding projectM
  - Push PCM buffers to `projectm_pcm_add_float`
  - Frame pacing and buffering strategy to match `set_fps`
- Beat handling
  - Use projectM’s sensitivity; optionally derive from detected BPM

### 3) Presets UX
- Visual improvements
  - Display current preset name; quick favorite/blacklist state
  - Random preset button, search field, and jump-to preset
- Collections
  - Favorites and blacklist views
  - Import/export preset lists (JSON)
- Auto-switching
  - Honor soft/hard cut durations; lock toggle from settings and hotkey
- Blacklist enforcement
  - Rebuild playlist excluding blacklisted items

### 4) Settings
- Add remaining toggles
  - UV/coordinate flip
  - VSync
  - Optional MSAA control
  - API version switch (v3/v4) with warning that restart is needed
- Hotkeys tab
  - Map to projectM SDL example defaults; allow user overrides
  - Export/import hotkey mappings
- Paths tab
  - Preset and texture paths (done), plus additional include dirs

### 5) Recording
- Rendering
  - Offscreen FBO render at target resolution and FPS
  - Option to separate render FPS from UI FPS
- Encoding
  - FFmpeg-based muxing to MP4 (H.264 + AAC) and MKV (H.264/HEVC + FLAC/Opus)
  - Choose CRF/bitrate and audio codec
- Audio source
  - From app playback pipeline or system capture
- UX
  - Recording panel with start/stop, output path, format, resolution, fps
  - Progress and disk space checks

### 6) Text overlay
- Display preset name, track metadata, elapsed time
- Rendering options
  - QPainter over FBO image or OpenGL text rendering (signed distance field)
- Styling
  - Font, size, shadow/outline, color theme
- Toggle and timeout (hide after N seconds)

### 7) Hotkeys
- Defaults modeled on `frontend-sdl-cpp`
  - Next/prev preset, random, lock, favorite, blacklist, toggle overlay, start/stop recording, fullscreen
- Settings tab to customize and save mappings
- Display cheat sheet in Help

### 8) Performance and compatibility
- OpenGL
  - CompatibilityProfile default; allow CoreProfile switch in settings
  - Mesh size controls; MSAA control
- Threading
  - Keep render fast; move I/O and decoding off GUI thread
- GPU/driver combos testing on Linux (Mesa, NVIDIA), Windows (WGL/ANGLE), macOS (Metal via Molten? Not required initially)

### 9) Packaging and distribution
- Linux: AppImage/Flatpak; .deb for Debian/Ubuntu
- Windows: MSYS2 build; installer via NSIS/Wix
- macOS: App bundle; codesigning if available
- Ship minimal preset pack, point to external packs for full collections

### 10) CI/CD
- GitHub Actions matrix
  - Linux/macOS/Windows build
  - Cache `external/projectm`
  - Run unit tests for config and preset manager
- Release artifacts
  - Upload binaries (AppImage/zip) on tags

### 11) Risks
- projectM API drift (v4 → v5)
- OpenGL compatibility on some drivers; consider fallback paths
- Audio sync and latency under varied systems
- Preset licensing and redistribution constraints

## Milestones and acceptance criteria

### M1: Baseline visualizer and settings (done/ongoing)
- Build v4 libprojectM automatically
- Renders idle preset by default; prev/next/favorite/blacklist UI
- Settings for visualizer parameters; persisted JSON
- Favorites/blacklist persisted and applied to UI state

Acceptance: App runs, shows visualizer, settings dialog changes affect rendering, favorites/blacklist persist across restarts.

### M2: Audio pipeline
- Decode and play common formats; audio out via PipeWire/PulseAudio
- Feed PCM to projectM; visible reaction to audio

Acceptance: Loading a track shows responsive visuals; playback controls work.

### M3: Preset UX polish
- Random/jump/search; favorites/blacklist views; import/export preset lists
- Blacklist applied to playlist building

Acceptance: Users can manage and navigate presets smoothly; blacklist is respected.

### M4: Recording and overlays
- Offscreen FBO rendering; FFmpeg muxing; configurable resolution/FPS/CRF
- Text overlay for preset/track

Acceptance: User records a clip with synced audio, overlay visible when enabled.

### M5: Hotkeys and portability
- Hotkeys implemented and configurable
- Build on Windows/macOS; basic packaging

Acceptance: Hotkeys usable and customizable; CI builds and packages for target platforms.

## Testing plan
- Unit tests (where feasible)
  - Config load/save round-trip
  - PresetManager favorites/blacklist
- Manual matrix
  - GPUs/drivers (Mesa/NVIDIA)
  - OpenGL core vs compatibility profiles
  - Different preset packs/paths
  - Various audio formats and sample rates
- Recording validation
  - Frame pacing accuracy; A/V sync; output bitrates
- Performance checks
  - CPU/GPU usage vs FPS; mesh size and MSAA variations

## Developer workflow notes
- Prefer local `external/projectm` checkout/submodule for offline work
- Toggle fetching via `-DNEONWAVE_FETCH_PROJECTM=ON/OFF`
- Change libprojectM tag via `-DNEONWAVE_PROJECTM_GIT_TAG=v4.1.0`
- For API v3 exploration, use the SDL example for reference: `https://github.com/projectM-visualizer/frontend-sdl-cpp`

## References
- projectM library: `https://github.com/projectM-visualizer/projectm`
- projectM SDL frontend example: `https://github.com/projectM-visualizer/frontend-sdl-cpp`
- Existing docs in this repo under `docs/` and `project_planning_drafts/docs/`
