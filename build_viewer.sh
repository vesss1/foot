#!/bin/bash
# Video Data Viewer - Linux/macOS Build Script

echo "========================================"
echo "Video Data Viewer - Build Script"
echo "========================================"
echo ""

# Check if qmake is available
if ! command -v qmake &> /dev/null; then
    echo "ERROR: qmake not found in PATH"
    echo ""
    echo "Please install Qt6:"
    echo "  Ubuntu/Debian: sudo apt-get install qt6-base-dev"
    echo "  macOS: brew install qt@6"
    echo ""
    exit 1
fi

echo "Found qmake:"
qmake --version
echo ""

# Check for OpenCV
if pkg-config --exists opencv4; then
    echo "Found OpenCV:"
    pkg-config --modversion opencv4
elif [ -d "/usr/local/include/opencv4" ]; then
    echo "Found OpenCV at /usr/local"
elif [ -d "/opt/homebrew/include/opencv4" ]; then
    echo "Found OpenCV at /opt/homebrew (Apple Silicon)"
else
    echo "WARNING: OpenCV not detected"
    echo "Please install OpenCV:"
    echo "  Ubuntu/Debian: sudo apt-get install libopencv-dev"
    echo "  macOS: brew install opencv"
    echo ""
fi

echo ""
echo "========================================"
echo "Building VideoDataViewer..."
echo "========================================"
echo ""

# Clean previous build
if [ -f "Makefile" ]; then
    echo "Cleaning previous build..."
    make clean 2>/dev/null
    rm -f Makefile
fi

# Run qmake
echo "Running qmake..."
qmake VideoDataViewer.pro
if [ $? -ne 0 ]; then
    echo "ERROR: qmake failed"
    exit 1
fi

# Build the project
echo ""
echo "Building project..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed"
    exit 1
fi

echo ""
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo ""
echo "Executable: ./VideoDataViewer"
echo ""
echo "To run:"
echo "  1. Ensure output.avi exists in foot-Function/output_videos/"
echo "  2. Run: ./VideoDataViewer"
echo ""

# Make executable if not already
chmod +x VideoDataViewer 2>/dev/null

# Check if we can run it
if [ -f "./VideoDataViewer" ]; then
    echo "Ready to run!"
    echo ""
    read -p "Would you like to run the viewer now? (y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        ./VideoDataViewer
    fi
fi
