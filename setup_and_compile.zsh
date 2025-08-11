#!/usr/bin/env zsh
set -euo pipefail

# Simple setup + build helper for NeonWave
# - Optionally clones/updates projectM into external/ if requested
# - Configures and builds the app

ROOT_DIR=${0:A:h}
cd "$ROOT_DIR"

# Defaults
BUILD_TYPE=${BUILD_TYPE:-Release}
JOBS=${JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}
FETCH_PROJECTM=${FETCH_PROJECTM:-ON}
PROJECTM_REPO=${PROJECTM_REPO:-https://github.com/projectM-visualizer/projectm.git}
PROJECTM_TAG=${PROJECTM_TAG:-v4.1.0}
BUILD_DIR=${BUILD_DIR:-$ROOT_DIR/build}

print_usage() {
  cat <<EOF
Usage: ./setup_and_compile.zsh [options]

Options (env vars or flags):
  --debug                Set build type to Debug (BUILD_TYPE=Debug)
  --release              Set build type to Release (BUILD_TYPE=Release)
  --jobs N               Parallel jobs (JOBS)
  --no-fetch-projectm    Do not fetch projectM (NEONWAVE_FETCH_PROJECTM=OFF)
  --repo URL             projectM repo URL (PROJECTM_REPO)
  --tag TAG              projectM tag/branch (PROJECTM_TAG)

Examples:
  ./setup_and_compile.zsh --debug
  JOBS=16 ./setup_and_compile.zsh --no-fetch-projectm
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --debug) BUILD_TYPE=Debug; shift ;;
    --release) BUILD_TYPE=Release; shift ;;
    --jobs) JOBS=$2; shift 2 ;;
    --no-fetch-projectm) FETCH_PROJECTM=OFF; shift ;;
    --repo) PROJECTM_REPO=$2; shift 2 ;;
    --tag) PROJECTM_TAG=$2; shift 2 ;;
    -h|--help) print_usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; print_usage; exit 1 ;;
  esac
done

mkdir -p "$BUILD_DIR"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DNEONWAVE_FETCH_PROJECTM="$FETCH_PROJECTM" \
  -DNEONWAVE_PROJECTM_GIT_REPO="$PROJECTM_REPO" \
  -DNEONWAVE_PROJECTM_GIT_TAG="$PROJECTM_TAG"

cmake --build "$BUILD_DIR" -j "$JOBS"

echo "\nBuild complete: $BUILD_DIR/neonwave"
