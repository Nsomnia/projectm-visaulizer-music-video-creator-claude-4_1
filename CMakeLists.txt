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
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Find SDL2
pkg_check_modules(SDL2 REQUIRED sdl2)

# Find audio libraries
pkg_check_modules(PULSE REQUIRED libpulse)

# ProjectM paths (assuming it's built in external/)
set(PROJECTM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/projectm/include")
set(PROJECTM_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/external/projectm/build")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${PROJECTM_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${PULSE_INCLUDE_DIRS}
)

# Link directories
link_directories(
    ${PROJECTM_LIBRARY_DIR}
    ${SDL2_LIBRARY_DIRS}
    ${PULSE_LIBRARY_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/Config.cpp
    src/gui/MainWindow.cpp
    src/gui/SettingsDialog.cpp
    src/gui/PlaylistWidget.cpp
    src/audio/AudioEngine.cpp
    src/visualizer/ProjectMWidget.cpp
    src/visualizer/PresetManager.cpp
    src/utils/FileUtils.cpp
    src/utils/StringUtils.cpp
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
    projectM-4
    ${SDL2_LIBRARIES}
    ${PULSE_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install desktop file
install(FILES resources/neonwave.desktop
    DESTINATION share/applications
)