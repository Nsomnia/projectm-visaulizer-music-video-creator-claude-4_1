#!/usr/bin/env bash
# Build helper to fetch and compile libprojectM (v4) with CMake.
# - Clones into external/projectm if missing
# - Checks out the requested ref
# - Configures and builds with playlist support
#
# Usage:
#   bash scripts/build_projectm.sh [--ref v4.1.0] [--repo URL] [--clean] [--jobs N] [--build-type Release]
#   bash scripts/build_projectm.sh --help

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Try to anchor to repo root if under git, else use parent dir of scripts/
if git -C "${SCRIPT_DIR}" rev-parse --show-toplevel >/dev/null 2>&1; then
  REPO_ROOT="$(git -C "${SCRIPT_DIR}" rev-parse --show-toplevel)"
else
  REPO_ROOT="${SCRIPT_DIR%/scripts}"
fi
EXTERNAL_DIR="${REPO_ROOT}/external"
PROJECTM_DIR="${EXTERNAL_DIR}/projectm"
BUILD_DIR="${PROJECTM_DIR}/build"

PROJECTM_REPO="https://github.com/projectM-visualizer/projectm.git"
PROJECTM_REF="v4.1.0"
BUILD_TYPE="Release"
JOBS="$(nproc || sysctl -n hw.ncpu || echo 4)"
CLEAN=0

print_usage() {
  cat <<EOF
Build libprojectM locally using CMake

Options:
  --repo URL         Git URL for projectM (default: ${PROJECTM_REPO})
  --ref REF          Git ref/tag/branch to checkout (default: ${PROJECTM_REF})
  --build-type TYPE  CMAKE_BUILD_TYPE (default: ${BUILD_TYPE})
  --jobs N           Parallel build jobs (default: ${JOBS})
  --clean            Remove existing build dir before configuring
  -h, --help         Show this help

Notes:
- This repository's top-level CMake already adds external/projectm via add_subdirectory,
  so you can simply run the normal build; use this script when you specifically want to
  prebuild libprojectM or refresh the external checkout.
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --repo) PROJECTM_REPO="$2"; shift 2 ;;
    --ref) PROJECTM_REF="$2"; shift 2 ;;
    --build-type) BUILD_TYPE="$2"; shift 2 ;;
    --jobs) JOBS="$2"; shift 2 ;;
    --clean) CLEAN=1; shift ;;
    -h|--help) print_usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; print_usage; exit 1 ;;
  esac
done

command -v cmake >/dev/null 2>&1 || { echo "Error: cmake is required" >&2; exit 1; }
command -v git >/dev/null 2>&1 || { echo "Error: git is required" >&2; exit 1; }

mkdir -p "${EXTERNAL_DIR}"

if [[ ! -d "${PROJECTM_DIR}/.git" ]]; then
  echo "[projectM] Cloning ${PROJECTM_REPO} -> ${PROJECTM_DIR}"
  git clone --depth 1 --branch "${PROJECTM_REF}" "${PROJECTM_REPO}" "${PROJECTM_DIR}" || {
    echo "Clone failed; attempting full clone then checkout..." >&2
    git clone "${PROJECTM_REPO}" "${PROJECTM_DIR}"
  }
else
  echo "[projectM] Updating existing repo at ${PROJECTM_DIR}"
  git -C "${PROJECTM_DIR}" fetch --tags --prune
fi

# Checkout desired ref
if [[ -n "${PROJECTM_REF}" ]]; then
  echo "[projectM] Checking out ${PROJECTM_REF}"
  git -C "${PROJECTM_DIR}" checkout --quiet "${PROJECTM_REF}" || {
    echo "Ref ${PROJECTM_REF} not found locally; attempting to fetch it..." >&2
    git -C "${PROJECTM_DIR}" fetch --tags origin "${PROJECTM_REF}:${PROJECTM_REF}"
    git -C "${PROJECTM_DIR}" checkout --quiet "${PROJECTM_REF}"
  }
fi

if [[ ${CLEAN} -eq 1 ]]; then
  echo "[projectM] Cleaning build directory ${BUILD_DIR}"
  rm -rf "${BUILD_DIR}"
fi
mkdir -p "${BUILD_DIR}"

# Configure
cmake -S "${PROJECTM_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
  -DBUILD_TESTING=OFF \
  -DENABLE_SDL_UI=OFF \
  -DENABLE_INSTALL=OFF \
  -DENABLE_SYSTEM_GLM=OFF \
  -DENABLE_SYSTEM_PROJECTM_EVAL=ON

# Build
cmake --build "${BUILD_DIR}" -j "${JOBS}"

echo "\n[projectM] Build completed. Artifacts are under: ${BUILD_DIR}"
echo "Libraries (if built as shared):"
find "${BUILD_DIR}" -maxdepth 3 -type f -name 'libprojectM-4*.so*' -o -name 'libprojectM-4*.dylib' -o -name 'projectM-4*.dll' || true

# Reminder about integration
cat <<EOF

Integration:
- Top-level CMake is already configured to build and link against external/projectm via add_subdirectory.
- You can now run the normal build for this project:
    cmake -S "${REPO_ROOT}" -B "${REPO_ROOT}/build"
    cmake --build "${REPO_ROOT}/build" -j "${JOBS}"
EOF
