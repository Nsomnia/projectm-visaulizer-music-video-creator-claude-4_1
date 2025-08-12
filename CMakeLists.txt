### CMakeLists.txt

cmake_minimum_required(VERSION 3.20)
project(neonwave VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable all warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets Multimedia)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Find SDL2
pkg_check_modules(SDL2 REQUIRED sdl2)

# Find audio libraries
pkg_check_modules(PULSE REQUIRED libpulse)

# Find FreeType for text rendering
pkg_check_modules(FREETYPE REQUIRED freetype2)

# Find GLM for math
find_package(glm REQUIRED)

# projectM integration options
option(NEONWAVE_FETCH_PROJECTM "Fetch projectM from Git if external/projectm is missing" ON)
set(NEONWAVE_PROJECTM_GIT_REPO "https://github.com/projectM-visualizer/projectm.git" CACHE STRING "projectM repository URL")
set(NEONWAVE_PROJECTM_GIT_TAG "v4.1.0" CACHE STRING "projectM Git tag/branch/ref to checkout")

# Add projectM: prefer local submodule/checkout at external/projectm; otherwise FetchContent
set(PROJECTM_LOCAL_DIR "${CMAKE_SOURCE_DIR}/external/projectm")
if (EXISTS "${PROJECTM_LOCAL_DIR}/CMakeLists.txt")
    message(STATUS "Using local projectM source at ${PROJECTM_LOCAL_DIR}")
    # Align options with our FetchContent defaults to avoid system deps
    set(ENABLE_SDL_UI OFF CACHE BOOL "" FORCE)
    set(ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(ENABLE_SYSTEM_GLM OFF CACHE BOOL "" FORCE)
    set(ENABLE_SYSTEM_PROJECTM_EVAL OFF CACHE BOOL "" FORCE)
    add_subdirectory("${PROJECTM_LOCAL_DIR}")
    set(PROJECTM_SOURCE_DIR "${PROJECTM_LOCAL_DIR}")
elseif(NEONWAVE_FETCH_PROJECTM)
    include(FetchContent)
    message(STATUS "Fetching projectM from ${NEONWAVE_PROJECTM_GIT_REPO} (${NEONWAVE_PROJECTM_GIT_TAG})")
    FetchContent_Declare(
        projectm
        GIT_REPOSITORY ${NEONWAVE_PROJECTM_GIT_REPO}
        GIT_TAG        ${NEONWAVE_PROJECTM_GIT_TAG}
        GIT_SHALLOW    TRUE
    )
    # Reduce optional components to shrink build
    set(ENABLE_SDL_UI OFF CACHE BOOL "" FORCE)
    set(ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(ENABLE_SYSTEM_GLM OFF CACHE BOOL "" FORCE)
    set(ENABLE_SYSTEM_PROJECTM_EVAL ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(projectm)
    # Expose source dir for includes if needed
    FetchContent_GetProperties(projectm)
    set(PROJECTM_SOURCE_DIR "${projectm_SOURCE_DIR}")
else()
    message(FATAL_ERROR "projectM not found at ${PROJECTM_LOCAL_DIR} and NEONWAVE_FETCH_PROJECTM is OFF")
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    # Prefer projectM's exported usage requirements, but also add direct include path for headers
    ${PROJECTM_SOURCE_DIR}/src/api/include
    ${SDL2_INCLUDE_DIRS}
    ${PULSE_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

# Link directories
link_directories(
    ${SDL2_LIBRARY_DIRS}
    ${PULSE_LIBRARY_DIRS}
    ${FREETYPE_LIBRARY_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/Config.cpp
    src/gui/MainWindow.cpp
    src/gui/SettingsDialog.cpp
    src/gui/PlaylistWidget.cpp
    src/core/audio/AudioEngine.cpp
    src/visualizer/ProjectMWidget.cpp
    src/visualizer/PresetManager.cpp
    src/visualizer/TextRenderer.cpp
    src/core/utils/FileUtils.cpp
    src/core/utils/StringUtils.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set Qt6 automoc
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Multimedia
    projectM-4
    projectM-4-playlist
    ${SDL2_LIBRARIES}
    ${PULSE_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Provide default preset/texture directories to the app at compile time
set(PROJECTM_DEFAULT_PRESETS_DIR "${PROJECTM_SOURCE_DIR}/presets")
set(PROJECTM_DEFAULT_TEXTURES_DIR "${PROJECTM_SOURCE_DIR}/textures")
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECTM_DEFAULT_PRESETS_DIR="${PROJECTM_DEFAULT_PRESETS_DIR}"
    PROJECTM_DEFAULT_TEXTURES_DIR="${PROJECTM_DEFAULT_TEXTURES_DIR}"
)

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install desktop file
install(FILES resources/neonwave.desktop
    DESTINATION share/applications
)