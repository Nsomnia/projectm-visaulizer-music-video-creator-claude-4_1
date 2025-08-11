
---

Session notes (latest attempts)

• Clarified naming: UI "Playlist" refers to audio queue; projectM preset manager remains "preset playlist" internally; UI label changed to "Audio Queue" to avoid confusion.
• Quickfix applied: ensured a preset is loaded before creating/connecting any preset playlist; later disabled preset playlist entirely during debug.
• Rendering paths iterated:
  • Direct render: projectm_opengl_render_frame to default framebuffer with minimal GL state changes (no forced disables). Still black.
  • Audio feed: increased PCM push frequency and size; tried 8× then 32× 1024-frame blocks per frame; then 44100-frame stereo per frame; maintained monotonic frame_time. No visible output.
  • Forced preset loading order: after window size set; tried bundled res/minimal_test.milk else idle:// preset.
  • FBO render path: created RGBA8 texture FBO + DEPTH24_STENCIL8 RBO; render via projectm_opengl_render_frame_fbo then compose with fullscreen quad shader. Added completeness check, guarded binds, and resized FBO on viewport changes. Still black; sometimes segfault.
• GL state alignment: removed aggressive glDisable calls; matched SDL sample ordering: bind default FB, viewport, clear color buffer, render; for FBO path, clear FBO then render then compose.
• Crash behavior: intermittent segfaults after switching to FBO path even with guards; indicates an internal GL/FBO/state expectation mismatch or invalid handle lifecycle.
• Next steps proposed:
  1) Use gdb to capture backtrace at segfault in vibestream; inspect frames in projectM renderer to pinpoint failure.
  2) Mirror external/frontend-sdl-cpp exact initialization and render calls (ProjectMWrapper) including any required GL loaders/state and audio callback cadence (e.g., 512–2048 frame pushes at ~50–100 Hz).
  3) Temporarily swap QOpenGLWidget for a raw GL context (QOpenGLWindow) to remove any implicit Qt blits; or test a minimal standalone SDL window using the same projectM instance to bisect Qt vs projectM issues.
  4) Add logging around projectm_load_preset_file return and preset switch callbacks; verify preset actually loads and isn’t failing silently.
  5) If Qt core profile conflicts with projectM shaders, try CompatibilityProfile strictly, or request the exact profile and version used by sdl-test-ui.
• Blockers: Despite feeding PCM and loading presets, projectM draws nothing in current Qt context; FBO path doesn’t resolve it; segfaults persist.
