# Football Analysis GUI

A Qt6 desktop application for analyzing football/soccer match videos using computer vision and machine learning.

## Overview

This application provides a user-friendly GUI for running football video analysis using Python-based computer vision algorithms. It tracks players, detects ball possession, calculates distances and speeds, and produces annotated video output with comprehensive statistics.

## Features

### Core Functionality
- **Video Analysis Pipeline**: Process football match videos with YOLO object detection
- **Player Tracking**: Track individual players throughout the match
- **Team Classification**: Automatic team identification based on jersey colors
- **Ball Possession Analysis**: Calculate possession percentages per team
- **Distance & Speed Metrics**: Measure distance covered and speed for each player
- **Camera Movement Compensation**: Adjust for camera panning and movement

### GUI Features
- **Tabbed Results Interface**:
  - **Summary Tab**: Quick overview and status
  - **Data Table Tab**: Player statistics in tabular format
  - **Video Output Tab**: Embedded video player with playback controls
  
- **Real-time Monitoring**:
  - Live stdout/stderr output from Python analysis
  - Progress updates and status messages
  - Non-blocking UI (remains responsive during analysis)

- **Automatic Result Loading**:
  - CSV data automatically loaded and displayed in table
  - JSON data as fallback format
  - Output video automatically loaded into media player

- **Video Playback**:
  - Native Qt Multimedia integration
  - Play/Pause and Stop controls
  - Embedded video widget

## Technology Stack

- **GUI Framework**: Qt 6.10.x (Qt Widgets + Qt Multimedia)
- **Language**: C++17
- **Analysis Backend**: Python 3.8+
- **Computer Vision**: OpenCV, YOLO (via Ultralytics)
- **Build System**: qmake

## Project Structure

```
foot/
├── FootAnalysisGUI.pro          # Qt project file
├── main.cpp                     # Application entry point
├── MainWindow.h                 # Main window header
├── MainWindow.cpp               # Main window implementation
├── BUILD_INSTRUCTIONS.md        # Detailed build guide
└── foot-Function/               # Python analysis backend
    ├── main.py                  # Main analysis pipeline
    ├── trackers/                # Object tracking
    ├── team_assigner/           # Team classification
    ├── camera_movement_estimator/
    ├── view_transformer/
    ├── speed_and_distance_estimator/
    ├── player_ball_assigner/
    ├── utils/                   # Utilities and data export
    ├── models/                  # YOLO models
    ├── input_videos/            # Sample input videos
    └── output_videos/           # Generated outputs
        ├── output_video.avi     # Annotated video
        ├── data_output.json     # Statistics (JSON)
        └── data_output.csv      # Statistics (CSV)
```

## Quick Start

### Prerequisites
- Qt Creator 18.0.0 or later
- Qt 6.10.x with Multimedia modules
- Python 3.8+ with dependencies (OpenCV, NumPy, PyTorch, Ultralytics)
- C++ compiler (MSVC on Windows, GCC/Clang on Linux/macOS)

### Building

#### On Windows (Qt Creator):
1. Open `FootAnalysisGUI.pro` in Qt Creator
2. Select Qt 6.10.x Desktop kit
3. Click Build (Ctrl+B)
4. Click Run (Ctrl+R)

#### On Linux:
```bash
# Install Qt6 dependencies
sudo apt-get install qt6-base-dev qt6-multimedia-dev

# Build
qmake6 FootAnalysisGUI.pro
make

# Run
./FootAnalysisGUI
```

For detailed build instructions, see [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md).

### Python Setup

Install Python dependencies:
```bash
cd foot-Function
pip install opencv-python numpy torch ultralytics
```

## Usage

1. **Launch the Application**
2. **Select Input Video**: Browse and choose a football match video file
3. **Select YOLO Model**: Browse and choose the trained model file (e.g., `best.pt`)
4. **Start Analysis**: Click "Start Analysis" and monitor progress in the log
5. **View Results**:
   - **Data Table**: View player statistics and possession percentages
   - **Video Output**: Play the annotated video with tracking overlays

## Output Files

The Python analysis generates three output files in `foot-Function/output_videos/`:

1. **output_video.avi**: Annotated video showing:
   - Player bounding boxes with team colors
   - Ball tracking
   - Speed and distance metrics overlay
   - Possession indicators

2. **data_output.csv**: Tabular data with:
   - Player IDs by team
   - Distance covered (meters)
   - Team possession percentages

3. **data_output.json**: Structured data including:
   - Per-player statistics
   - Team-level summaries
   - Possession breakdown

## Architecture

### GUI Layer (Qt/C++)
- **MainWindow**: Primary application window
- **QProcess**: Asynchronous Python process execution
- **QMediaPlayer**: Video playback
- **QTableWidget**: Data visualization

### Analysis Layer (Python)
- **VideoAnalysisPipeline**: Main orchestrator
- **Tracker**: YOLO-based object detection and tracking
- **TeamAssigner**: K-means clustering for team colors
- **CameraMovementEstimator**: Optical flow for camera motion
- **ViewTransformer**: Perspective transformation
- **SpeedAndDistance_Estimator**: Metric calculations
- **PlayerBallAssigner**: Possession detection

### Data Flow
1. User selects video and model via GUI
2. GUI launches Python process with QProcess
3. Python performs analysis, outputs to files
4. GUI monitors stdout/stderr in real-time
5. On completion, GUI automatically loads results:
   - Parses CSV/JSON into table widget
   - Loads video into media player
6. User views results in tabbed interface

## Key Implementation Details

### Asynchronous Execution
- Python runs in separate QProcess
- UI remains responsive during analysis
- Real-time output capture via signals/slots

### Automatic Result Loading
- `onProcessFinished()` slot triggered on completion
- Files loaded from known output directory
- Graceful handling of missing files

### Video Playback
- Qt Multimedia provides native codec support
- QVideoWidget embedded in tab interface
- Simple play/pause/stop controls

### Data Display
- CSV parsing with QTextStream
- JSON parsing with QJsonDocument
- Dynamic table population
- Formatted display with summary sections

## Troubleshooting

### Common Issues

**Qt Multimedia not found**
- Ensure Qt Multimedia modules are installed
- Check Qt installation includes multimedia support

**Python process fails to start**
- Verify Python is in system PATH
- Check Python dependencies are installed

**Video playback shows black screen**
- Install system codecs (K-Lite on Windows, gstreamer on Linux)
- Verify video file is valid and not corrupted

**Results not loading**
- Check Analysis Log for Python errors
- Verify `foot-Function/output_videos/` directory exists
- Ensure analysis completed successfully (exit code 0)

For more troubleshooting tips, see [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md).

## Development

### Code Structure

**MainWindow.h/cpp**:
- `setupUI()`: Constructs UI layout with tabs and widgets
- `onStartAnalysis()`: Launches Python process
- `onProcessFinished()`: Handles completion, loads results
- `loadAndDisplayCSV()`: Parses and displays CSV data
- `loadAndDisplayJSON()`: Parses and displays JSON data  
- `loadAndPlayVideo()`: Loads video into media player

**main.cpp**:
- Application entry point
- Creates and shows MainWindow

**FootAnalysisGUI.pro**:
- qmake project configuration
- Links Qt modules (core, gui, widgets, multimedia)

### Extending the Application

To add new features:
1. Update Python scripts to output additional data
2. Modify `loadAndDisplayCSV()` or `loadAndDisplayJSON()` to parse new data
3. Add UI elements in `setupUI()` to display new information
4. Connect signals/slots for interactivity

## License

See repository license for details.

## Acknowledgments

This project combines Qt desktop application development with Python-based computer vision to provide an accessible interface for football video analysis.

