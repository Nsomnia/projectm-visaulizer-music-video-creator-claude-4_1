# Preset Management System

## Preset Navigation

### Next Preset
- Loads next available preset
- Skips presets in ignore list
- Limited to favorites in recording mode

### Previous Preset
- Loads previous preset from history
- Maintains preset history stack
- Bounded history size (default: 50)

## Preset Lists

### Favorites
- User-selected preferred presets
- Stored in `$HOME/.config/vibestream/favorites.txt`
- Only presets used in recording mode when populated

### Ignored
- User-blacklisted presets
- Stored in `$HOME/.config/vibestream/ignored.txt`
- Automatically skipped during navigation

## History Management

### Preset History
- Stack-based navigation
- Automatic history pruning
- Persistent across sessions

## File Format

### List Files (.txt)
- One preset path per line
- Comments start with `#`
- Empty lines ignored
- Relative paths resolved from preset directory

## Preset Selection Algorithm

1. If recording and favorites not empty:
   a. Select random from favorites
2. Else:
   a. Get all available presets
   b. Remove ignored presets
   c. Select random from remaining
3. Update history
4. Load selected preset