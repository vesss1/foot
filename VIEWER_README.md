# Video and Data Viewer - Qt Application

## Overview

This is a complete Qt 6.10.1 C++ application that displays an AVI video file alongside CSV/JSON data in a synchronized viewer. The application provides real-time video playback with data display in a table or text format.

## Features

- **Video Playback**: Displays AVI video files using OpenCV
- **Real-time Updates**: Video frames update in real-time at the original FPS
- **Data Display**: Shows CSV or JSON data in either:
  - Table view with sortable columns
  - Raw text view for inspection
- **Playback Controls**: Play/Pause and Reset buttons
- **Responsive Layout**: Resizable splitter between video and data sections
- **Auto-detection**: Automatically finds and loads output.avi and data files

## Project Structure

```
foot/
├── VideoDataViewer.h           # Header file for the viewer class
├── VideoDataViewer.cpp         # Implementation of the viewer
├── viewer_main.cpp             # Main entry point
├── VideoDataViewer.pro         # Qt project file with OpenCV configuration
├── create_test_video.py        # Python script to generate test video
└── foot-Function/
    └── output_videos/
        ├── output.avi          # Video file
        ├── output_data.csv     # CSV data file
        └── output_data.json    # JSON data file
```

## Prerequisites

### Required Software

1. **Qt 6.10.1** or compatible version
   - Download from: https://www.qt.io/download
   - Components needed: Qt Widgets, Qt Core, Qt GUI

2. **OpenCV 4.x**
   - Windows: Download from https://opencv.org/releases/
   - Linux: `sudo apt-get install libopencv-dev`
   - macOS: `brew install opencv`

3. **C++17 Compatible Compiler**
   - Windows: MSVC 2019 or later
   - Linux: GCC 7+ or Clang 5+
   - macOS: Xcode 10+

### OpenCV Installation Details

#### Windows
1. Download OpenCV from https://opencv.org/releases/
2. Extract to `C:\opencv\` (or update paths in .pro file)
3. Add OpenCV bin directory to PATH:
   - `C:\opencv\build\x64\vc16\bin`

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install libopencv-dev
```

#### macOS
```bash
brew install opencv
```

## Building the Application

### Method 1: Using Qt Creator (Recommended)

1. Open Qt Creator
2. File → Open File or Project
3. Select `VideoDataViewer.pro`
4. Configure the project for your Qt kit
5. Build → Build Project
6. Run → Run

### Method 2: Command Line with qmake

```bash
# Navigate to project directory
cd /path/to/foot

# Generate Makefile
qmake VideoDataViewer.pro

# Build
make

# Run (Linux/macOS)
./VideoDataViewer

# Run (Windows)
VideoDataViewer.exe
```

### Method 3: Command Line with CMake (alternative)

If you prefer CMake, you can create a CMakeLists.txt file:

```cmake
cmake_minimum_required(VERSION 3.16)
project(VideoDataViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(OpenCV REQUIRED)

add_executable(VideoDataViewer
    viewer_main.cpp
    VideoDataViewer.cpp
    VideoDataViewer.h
)

target_link_libraries(VideoDataViewer
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${OpenCV_LIBS}
)
```

## Usage

### Running the Viewer

1. **Prepare your data**: Ensure you have:
   - A video file: `foot-Function/output_videos/output.avi`
   - A data file (optional): `output_data.csv` or `output_data.json`

2. **Launch the application**: The viewer will automatically:
   - Look for video in `foot-Function/output_videos/output.avi`
   - Look for CSV or JSON files in the same directory
   - Start playing the video automatically

3. **Controls**:
   - **Play/Pause**: Toggle video playback
   - **Reset**: Return to the first frame
   - **Display Mode**: Switch between Table View and Raw Text

### Creating Test Data

Run the included Python script to generate sample data:

```bash
python3 create_test_video.py
```

This creates:
- `output.avi` - A 3-second test video with moving objects
- `output_data.csv` - Sample tracking data
- `output_data.json` - Sample JSON analysis results

## Configuration

### Adjusting OpenCV Paths

Edit `VideoDataViewer.pro` if your OpenCV installation is in a different location:

**Windows**:
```qmake
INCLUDEPATH += C:/opencv/build/include
LIBS += -LC:/opencv/build/x64/vc16/lib -lopencv_world4101
```

**Linux**:
```qmake
PKGCONFIG += opencv4
```

**macOS**:
```qmake
INCLUDEPATH += /usr/local/include/opencv4
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_videoio
```

### Changing Default Video Location

Edit `VideoDataViewer.cpp` line ~47 to change the default path:

```cpp
QString outputDir = QDir::currentPath() + "/foot-Function/output_videos";
```

## Data File Formats

### CSV Format

```csv
Frame,Time,Objects_Detected,Ball_Position_X,Ball_Position_Y,Player_Count
0,0.00,5,320,240,4
1,0.03,5,325,238,4
...
```

- First row must be headers
- Comma-separated values
- Will be displayed in table format

### JSON Format

**Array of Objects**:
```json
[
  {
    "frame": 0,
    "timestamp": 0.00,
    "players": 4,
    "ball_detected": true
  },
  ...
]
```

**Single Object (Key-Value)**:
```json
{
  "video_info": {
    "total_frames": 100,
    "fps": 30
  },
  "analysis_summary": {
    "total_players": 22
  }
}
```

## Troubleshooting

### OpenCV Not Found

**Error**: `opencv2/opencv.hpp: No such file or directory`

**Solution**: 
- Verify OpenCV is installed
- Update INCLUDEPATH in .pro file
- Linux: Try `pkg-config --cflags opencv4`
- Windows: Ensure OpenCV bin is in PATH

### Video Won't Play

**Error**: Video label shows "Failed to open video file"

**Solutions**:
- Check video file exists at expected path
- Verify AVI codec compatibility (XVID recommended)
- Try different video file
- Check OpenCV has video codec support

### Qt Version Issues

**Error**: `QWidget: undefined reference`

**Solution**:
- Ensure Qt Widgets module is included
- Check QT += widgets in .pro file
- Verify Qt installation includes Widgets

### Build Errors on Windows

**Error**: Cannot open opencv library files

**Solution**:
- Match OpenCV build architecture (x64/x86) with Qt kit
- Use correct Visual Studio version (vc15/vc16)
- Debug vs Release library names differ (add 'd' suffix for debug)

## Architecture

### Class Structure

**VideoDataViewer**
- Inherits from QMainWindow
- Manages video playback with OpenCV VideoCapture
- Handles data file loading and parsing
- Updates UI in real-time via QTimer

### Key Components

1. **Video Display**: QLabel with scaled QPixmap from cv::Mat
2. **Data Display**: QTableWidget for structured data, QTextEdit for raw
3. **Playback Control**: QTimer-based frame updates at original FPS
4. **Data Parsing**: Supports both CSV and JSON formats

### Frame Update Flow

```
QTimer::timeout() 
  → updateFrame()
  → cv::VideoCapture::read()
  → displayFrame(cv::Mat)
  → Convert BGR to RGB
  → Create QImage
  → Update QLabel
```

## Extending the Application

### Adding More Data Formats

Edit `loadDataFile()` in VideoDataViewer.cpp:

```cpp
if (extension == "xml") {
    loadXMLData(dataPath);
    return true;
}
```

### Custom Video Controls

Add slider for seeking:

```cpp
QSlider *seekSlider = new QSlider(Qt::Horizontal);
connect(seekSlider, &QSlider::valueChanged, this, &VideoDataViewer::onSeek);
```

### Real-time Data Sync

Link video frame to data row:

```cpp
void VideoDataViewer::updateFrame() {
    // ... existing code ...
    
    // Highlight corresponding data row
    if (currentFrameIndex < dataRows.size()) {
        dataTable->selectRow(currentFrameIndex);
    }
}
```

## Performance Notes

- Video decoding is done on the main thread; consider threading for large videos
- Table updates are optimized with setUpdatesEnabled()
- Memory usage scales with video resolution and frame buffer

## License

This code is provided as an example implementation for educational purposes.

## Support

For issues related to:
- **Qt**: https://doc.qt.io/
- **OpenCV**: https://docs.opencv.org/
- **This code**: Check the main repository for issues/discussions

## Version History

- **1.0** - Initial implementation with video playback and CSV/JSON support
  - Qt 6.10.1 compatibility
  - OpenCV 4.x integration
  - Real-time frame updates
  - Dual data display modes
