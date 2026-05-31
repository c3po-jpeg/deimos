#!/bin/bash
set -e

RUN=false
for args in "$@"; do
    if [ "$args" == "--run" ]; then
        RUN=true
        break
    fi
done

echo "Building Enceladus..."

if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

if ! command -v cmake >/dev/null 2>&1; then
    echo "Error: cmake not found in PATH."
    echo "Install with: pacman -S mingw-w64-ucrt-x86_64-cmake"
    exit 1
fi

# ---------------------------------------------------------------------------
# Pick a make command (MSYS2 sometimes names it mingw32-make)
# ---------------------------------------------------------------------------
MAKE_CMD=""
if [ -n "$MSYSTEM" ]; then
    if command -v make >/dev/null 2>&1; then
        MAKE_CMD=make
    elif command -v mingw32-make >/dev/null 2>&1; then
        MAKE_CMD=mingw32-make
    else
        echo "Error: make not found. Install with: pacman -S mingw-w64-ucrt-x86_64-make"
        exit 1
    fi
fi

# ---------------------------------------------------------------------------
# CMake generator
# ---------------------------------------------------------------------------
GEN_ARGS=()
if [ -n "$MSYSTEM" ]; then
    case "$MSYSTEM" in
        MINGW*|UCRT*|msys)
            echo "Detected MSYS2 environment ($MSYSTEM), forcing Unix Makefiles generator"
            GEN_ARGS+=("-G" "Unix Makefiles")
            GEN_ARGS+=("-DCMAKE_C_COMPILER=gcc" "-DCMAKE_CXX_COMPILER=g++")
            ;;
    esac
fi

echo "Configuring CMake..."
if [ -n "$MAKE_CMD" ]; then
    cmake "${GEN_ARGS[@]}" -DCMAKE_MAKE_PROGRAM="$MAKE_CMD" ..
else
    cmake "${GEN_ARGS[@]}" ..
fi

echo "Building project..."
cmake --build .

# ---------------------------------------------------------------------------
# Compile shaders — use glslc from MSYS2/UCRT64 or whatever is on PATH
# ---------------------------------------------------------------------------
if command -v glslc >/dev/null 2>&1; then
    echo "Compiling shaders..."
    mkdir -p shaders
    glslc ../shaders/shader.vert -o shaders/shader.vert.spv
    glslc ../shaders/shader.frag -o shaders/shader.frag.spv
    glslc ../shaders/shadow.vert -o shaders/shadow.vert.spv
    echo "Shaders compiled to build/shaders/"
else
    echo "Warning: glslc not found, shaders not compiled."
    echo "Install with: pacman -S mingw-w64-ucrt-x86_64-shaderc"
fi

cd ..

if [ "$RUN" = true ]; then
    echo "Running Enceladus..."
    ./build/src/enceladus
fi