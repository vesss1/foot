#!/bin/bash
# Build verification script for Football Analysis Qt GUI

echo "=========================================="
echo "Football Analysis Qt GUI - Build Script"
echo "=========================================="
echo ""

# Check prerequisites
echo "Checking prerequisites..."
echo ""

# Check Qt6
if command -v qmake6 &> /dev/null || pkg-config --exists Qt6Core 2>/dev/null; then
    echo "✓ Qt6 found"
    QT_FOUND=1
else
    echo "✗ Qt6 not found"
    echo "  Install Qt6: sudo apt-get install qt6-base-dev qt6-base-dev-tools"
    QT_FOUND=0
fi

# Check CMake
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1)
    echo "✓ CMake found: $CMAKE_VERSION"
    CMAKE_FOUND=1
else
    echo "✗ CMake not found"
    CMAKE_FOUND=0
fi

# Check C++ compiler
if command -v g++ &> /dev/null; then
    GCC_VERSION=$(g++ --version | head -n1)
    echo "✓ C++ compiler found: $GCC_VERSION"
    GCC_FOUND=1
else
    echo "✗ C++ compiler not found"
    GCC_FOUND=0
fi

# Check Python3
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version)
    echo "✓ Python3 found: $PYTHON_VERSION"
    PYTHON_FOUND=1
else
    echo "✗ Python3 not found"
    PYTHON_FOUND=0
fi

echo ""
echo "=========================================="

# Build instructions
if [ $QT_FOUND -eq 0 ]; then
    echo ""
    echo "To install Qt6 on Ubuntu/Debian:"
    echo "  sudo apt-get update"
    echo "  sudo apt-get install qt6-base-dev qt6-base-dev-tools"
    echo ""
    echo "Alternatively, download Qt Creator from:"
    echo "  https://www.qt.io/download-qt-installer"
    exit 1
fi

# Build with CMake
echo ""
echo "Building with CMake..."
echo "=========================================="

# Create build directory
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo "Removing old build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo ""
echo "Configuring..."
cmake .. || exit 1

echo ""
echo "Building..."
cmake --build . || exit 1

echo ""
echo "=========================================="
echo "Build successful!"
echo ""
echo "To run the application:"
echo "  cd build/bin"
echo "  ./FootAnalysisGUI"
echo ""
echo "Or from the root directory:"
echo "  ./build/bin/FootAnalysisGUI"
echo "=========================================="
