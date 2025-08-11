Content mirrored for search engine indexing from:

https://github.com/projectM-visualizer/projectm/wiki/User-Manual-%28Standalone%2C-v3.1.x%29
Why does this service exist?
📅 Last Modified: Fri, 24 Jan 2025 15:33:57 GMT

User Manual (Standalone, v3.1.x) - projectM-visualizer/projectm GitHub Wiki
projectM 3.1.x User Manual for Standalone Applications
This manual covers the following standalone applications developed by the projectM core team and released with projectM versions up to 3.1.12:

The SDL-based, UI-less application available on the GitHub releases page.
The Qt-based frontend for Linux.
❗ Note: this document does not cover the new SDL2 frontend available in the frontend-sdl2 repository. Please see the frontend-sdl2 Wiki for up-to-date documentation of this application.

Installing
Steam
To install the Steam version, simply add the app to your Steam account and install/run it. Editing the configuration is identical to the other releases.

Windows Standalone
Download the latest available release ZIP (e.g. projectM-sdl-x64-win.zip) from the projectM Releases page. Unpack the archive somewhere and run projectMSDL.exe from the main directory.

macOS Standalone
Download the latest available installer PKG (e.g. projectM-macOS.pkg) from the projectM Releases page. Run it in Finder to install projectM on your Mac.

Note: Future releases will use DMG files and the more convenient drag and drop installation.

Linux Standalone
Currently, projectM does not provide a binary Linux version on the Releases page due to the heterogeneous Linux environment. You can either look in your distribution's package manager if there is a projectM package available for installation or try to compile it yourself.

The projectM team is currently evaluating options to provide a binary version that works on most recent Linux (x86_64) variants.

Linux Qt UI
The Qt UI might also be available in your distribution package manager, but as it is an optional component, you'll mostly be out of luck. If you want to give it a try, you'll need to compile it yourself.

The projectM team is working on bringing this UI to other platforms as well as providing ready-to-use releases.

Configuring projectM
The Qt UI comes with a configuration dialog that lets you edit and save the projectM configuration. While this is the easiest way, this application is probably not available to you. For all other releases, you need to change the configuration file.

Important note: Be aware that the projectM applications will overwrite the file and during this process remove any comments and emtpy lines from the file. If you want to switch between different settings, it's better to keep multiple copies of the file and replace the config.inp file before starting projectM.

Windows
On Windows, you will find projectM's configuration file config.inp in the following locations:

Past/current releases (up to 3.x): %USERDIR%\.projectM\
Future releases (4.0+): %APPDATA%\projectM\
Chances are high neither the directory nor the config.inp file exist. In this case, create the directory and copy this template into it.

Linux and macOS
On Linux and macOS, the configuration file will be located in each user's home directory in a hidden directory ~/.projectM. If this directory doesn't exist, you can create it with your favorite file manager or using the following terminal command:

mkdir ~/.projectM
If the configuration file is not already there, you can download and copy this template into it.

Hotkey reference
On macOS, use the ⌘ (command) key instead of the control key.

Please note that some older releases of projectM did not display any on-screen menu or messages. In this case, some shortcuts will either not work at all or display no feedback.

Application
Ctrl-F: Toggle full screen mode. Might need to be pressed twice if the window was set to fullscreen using OS shortcuts/buttons.
Ctrl-I: Select next audio input device.
Ctrl-M: Move window to next monitor. Does not work on all operating systems.
Ctrl-Q: Quits the application.
Ctrl-S: Stretch window over all monitors.
Up: Increase beat sensitivity by 0.01 (max 5)
Down: Decrease beat sensitivity by 0.01 (min 0)
Information
F1 or H: Show a help screen with the most important hotkeys.
F3: Show preset name
F4: Show rendering statistics and beat detection values.
F5: Show FPS.
Preset switching
L or Space: Toggle locking the current preset.
N: Next preset (hard transition).
P: Previous preset (hard transition).
R: Jump to random preset.
Y: Toggle preset shuffle.
Shift-N: Next preset (soft transition).
Shift-P: Previous preset (soft transition).
Mouse Scroll Down: Next preset (hard transition).
Mouse Scroll Up: Previous preset (hard transition).
Preset search and navigation
Except M and Return, the following hotkeys are only valid if search mode is enabled or the preset navigation is displayed.

M: Open preset navigation menu.
Return: Enter/leave text search mode.
Escape: Leave search mode or preset navigation menu.
Up: Select previous preset in the list.
Down: Select next preset in the list.
PageUp: Jump up half a page in the list.
PageDown: Jump down half a page in the list.
Home: Jump to the beginning of the list.
End: Jump to the end of the list.
Qt UI only
Mouse Right: Toggle application UI visibility (menu and playlist).
+: Increase preset rating by 1.
-: Decrease preset rating by 1.
Configuration reference
The configuration file uses a very simple "Name = Value" syntax. Most parameter names contain spaces, so this is the valid syntax:

Preset Path = /usr/share/projectM/presets
Comments can be placed in separate lines or after any setting using the hash # character:

Mesh X = 244   # Per-pixel mesh horizontal resolution
# Another comment
If "Value" says "Boolean", a (case-insensitive) value of 0, N, NO, F, FALSE or NONE is interpreted as "not set" or "disabled". Any other value, including an empty value, is considered as "set" or "enabled".

Aspect Correction
Value: Boolean
Default: True
Example: Aspect Correction = yes

Enabled aspect correction in presets that support it. This is generally used to make shapes not appear stretched, e.g. circles or squares not being squished in one direction.

Beat Sensitivity
Value: Any decimal number >= 0.0 and <= 5.0 Default: 1.0
Example: Beat Sensitivity = 0.8

Set the beat sensitivity threshold, used by most presets to perform on-beat actions like pulsing or changing rotation.

Easter Egg Parameter
Value: Any decimal number >= 0.0
Default: 0.0
Example: Easter Egg Parameter = 1

Being the most mystic and opaque parameter in projectM, this value will be fed as "sigma" or "standard deviation" value into a gaussian-distributed random number generator that determines (and randomizes) the actual display time of a preset between 1 and 60 seconds. The "mean" value of the function is the Preset Duration documented below.

If in doubt, leave it at 0.

To put it in simple words: If the easter egg parameter is 0.0, the random number generator will always return the value preset duration. For any higher value, it flattens the distribution curve, so the actual duration will always be less than the set duration.

If interested, you can read more about Normal Distribution on Wikipedia.

FPS
Value: Any integer number >= 1
Default: 35
Example: FPS = 60

Sets the target/maximum FPS value projectM should render at. The real FPS may be lower, either because rendering isn't fast enough to reach the requested framerate or due to vertical syncing.

Fullscreen
Value: Boolean
Default: False
Example: Fullscreen = yes

If true, the application will start in fullscreen mode. If false, it is displayed as a resizable window.

This setting might have no effect if the OS or application doesn't support either windowed or fullscreen modes.

Hard Cuts Enabled
Value: Boolean
Default: False
Example: Hard Cuts Enabled = yes

A hard cut is a beat-induced immediate transition to the next preset that might happen before the preset display duration is reached. Also see Hard Cut Duration and Hard Cut Sensitivityto fine-tune the behaviour.

Hard Cut Duration
Value: Any decimal number >= 0.0
Default: 60.0
Example: Hard Cut Duration = 10.5

Minimum number of seconds before a "hard cut" is performed. Only used if hard cuts are enabled.

Hard Cut Sensitivity
Value: Any decimal number >= 0.0
Default: 1.0
Example: Hard Cut Sensitivity = 0.8

Beat sensitivity value to perform a "hard cut". It will be performed only after the number of seconds set in Hard Cut Duration have passed. Only used if hard cuts are enabled.

Menu Font
Value: Path to a TTF file
Default: OS-dependent path, VeraMono.ttf
Example: Menu Font = /usr/share/projectM/fonts/VeraMono.ttf

TrueType font used for rendering the built-in on-screen menu.

Menu rendering with custom fonts is currently not implemented in projectM. The setting is ignored.

Mesh X
Value: Any integer number >= 1
Default: 32
Example: Mesh X = 192

Horizontal resolution of the per-pixel equation drawing grid.

This grid basically splits the rendering area into this number of columns and each grid field is then calculated and drawn individually. Setting the value too high will drastically decrease rendering performance.

This setting does not affect the actual rendering resolution.

Mesh Y
Value: Any integer number >= 1
Default: 24
Example: Mesh X = 144

Vertical resolution of the per-pixel equation drawing grid.

This grid basically splits the rendering area into this number of rows and each grid field is then calculated and drawn individually. Setting the value too high will drastically decrease rendering performance.

This setting does not affect the actual rendering resolution.

Preset Duration
Value: Any decimal number >= 1.0
Default: 15.0
Example: Preset Duration = 60.0

The time in seconds a presets should be displayed before switching automatically to the next in the playlist.

In the current version, the maximum display time is 60 seconds, no matter how high this value is set to. This might change in future releases, removing the upper limit.

Preset Path
Value: Path to the projectM presets
Default: OS-dependent path, /usr/local/share/projectM/presets on most Linux installations.
Example: Preset Path = C:\Users\Administrator\Documents\projectM Presets

The given path is scanned recursively for both presets and textures.

Having too many files under this path will increase startup time. Also make sure not to place too many or large images under this path as projectM currently loads all images into memory on startup, which can consume large amounts of memory in addition to long load times.

Shuffle Enabled
Value: Boolean
Default: True
Example: Shuffle Enabled = NO

If set to a true value, projectM will choose the next preset randomly from the playlist instead of skipping to the next item in the list.

Smooth Preset Duration
Value: Any decimal number >= 1.0
Default: 10.0
Example: Smooth Preset Duration = 0.5

Time it takes to smoothly blend between two presets on soft cuts.

Note: Was named Smooth Transition Duration in earlier versions.

Soft Cut Ratings Enabled
Value: Boolean
Default: False
Example: Soft Cut Ratings Enabled = 1

If enabled and shuffle is on, preset ratings are used to prefer highly rated presets and display low rated presets ledd frequently. Only has an effect if the integrating application supports preset ratings.

Texture Size
Value: Any integer number >= 1 and a power of 2
Default: 512
Example: Texture Size = 2048

The size of the internal texture projectM renders to.

Note: This setting is currently not used as projectM currently renders directly into the current OpenGL window, using the window size.

Title Font
Value: Path to a TTF file
Default: OS-dependent path, Vera.ttf
Example: Menu Font = /usr/share/projectM/fonts/Vera.ttf

TrueType font used for rendering the song title on screen.

Title rendering is currently not implemented in projectM. The setting is ignored.

Window Height
Value: Any integer number >= 1
Default: 512
Example: Window Height = 1080

Initial height of the projectM window, including title bar and borders.

Window Width
Value: Any integer number >= 1
Default: 512
Example: Window Width = 1920

Initial width of the projectM window, including borders.

Troubleshooting
Below are some frequent issues with projectM you may encounter and possible fixes.

If these do not answer or solve your specific problem, you will almost always find help on our Discord server. If you think your issue is an actual bug in projectM, feel free to open a bug ticket on GitHub.

The framerate is very low
The reason can be either your hardware being too slow or other, system and configuration related values are off:

Check if 3D hardware acceleration is enabled, e.g. you are using vendor drivers on Linux and not running projectM in a VM.
Check if your Mesh X and/or Mesh Y configuration settings aren't too high (above 200) or too low (below 5). Using Mesh X = 64 and Mesh Y = 32 is a safe value.
Check if something else is heavily using your CPU or GPU.
projectM does not start from within an RDP session
This is a known limitation with the Remote Desktop Service on Windows. It can run DirectX-based applications just fine, but only reports OpenGL 1.1 compatibility to 3D applications using this API. which projectM does. This OpenGL version does not even support shaders and thus is not sufficient to run projectM.

Either run projectM locally or use a third-party remote desktop client like NoMachine.

There are occasional framerate hitches/drops
Earlier projectM versions (up to all 3.x releases) had several performance issues that could lead to framerate drops on some systems. First try to update to a 4.x release.

In addition to that, mostly users running projectM on AMD Ryzen CPUs experience these drops. This is a known issue with the CPU, but there is no definitive fix to that for now. Other OpenGL-driven games or applications might also be affected.

projectM takes a very long to start
projectM scans for presets and textures on startup, so if you have a slow hard disk, load the files from a network share or have a huge preset collection and/or a large number of images in the preset dir, this will heavily increase projectM startup time. Here are a few suggestions to fix the issue:

Reduce the number of presets in your preset dir.
Look in your preset dir if it contains thumbnail images for each preset - some collections include them in the archive. Delete all unnecessary image files.
Check if there are some huge images/textures in your preset dir and remove them or replace them with smaller versions. These will take time to load and consume a lot of RAM.
projectM only displays the "M" logo preset after startup
If projectM only displays the idle preset, and you cannot switch to any other preset, check if your Preset Path configuration setting is set to the correct directory.

If projectM switches to other presets after a short time, this is normal behaviour. There is currently no option to skip the logo being shown as the initial preset.

 Add a custom footer
⚠️ **GitHub.com Fallback** ⚠️
🗂️ Page Index for this GitHub Wiki