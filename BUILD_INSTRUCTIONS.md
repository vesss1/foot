# Build Instructions for Foot Analysis GUI

## Prerequisites

### Required Software
- **Qt Creator 18.0.0** or later
- **Qt 6.10.x** (or compatible Qt6 version)
- **Qt Modules Required**:
  - Qt Core
  - Qt Gui
  - Qt Widgets
  - Qt Multimedia
  - Qt MultimediaWidgets
- **C++ Compiler**: MSVC 2019/2022 (Windows), GCC/Clang (Linux), Clang (macOS)
- **Python 3.8+** with required packages for the foot-Function analysis

### Python Dependencies
The Python analysis script (`foot-Function/main.py`) requires:
- OpenCV (cv2)
- NumPy
- PyTorch (for YOLO model)
- Other dependencies listed in foot-Function requirements

## Building on Windows (Qt Creator)

### Method 1: Using Qt Creator IDE

1. **Open the Project**:
   - Launch Qt Creator 18.0.0
   - Go to File → Open File or Project
   - Navigate to and select `FootAnalysisGUI.pro`

2. **Configure the Kit**:
   - Select a Qt 6.10.x Desktop kit (MSVC 2019/2022 64-bit recommended)
   - Ensure Qt Multimedia modules are available in your Qt installation
   - Click "Configure Project"

3. **Build the Project**:
   - Click the "Build" button (hammer icon) or press `Ctrl+B`
   - Or select Build → Build Project "FootAnalysisGUI"
   - Build output will be in the build directory

4. **Run the Application**:
   - Click the "Run" button (green play icon) or press `Ctrl+R`
   - Or select Build → Run

### Method 2: Command Line Build (Windows)

```cmd
:: Navigate to project directory
cd path\to\foot

:: Set up Qt environment (adjust path to your Qt installation)
call "C:\Qt\6.10.0\msvc2019_64\bin\qtenv2.bat"

:: Generate Makefile
qmake FootAnalysisGUI.pro

:: Build
nmake

:: Run (from build directory)
release\FootAnalysisGUI.exe
```

## Building on Linux

### Install Qt6 Dependencies

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-multimedia-dev \
                     libqt6multimedia6 libqt6multimediawidgets6 \
                     build-essential

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qtmultimedia-devel

# Arch
sudo pacman -S qt6-base qt6-multimedia
```

### Build from Command Line

```bash
# Navigate to project directory
cd /path/to/foot

# Generate Makefile
qmake6 FootAnalysisGUI.pro
# or: qmake FootAnalysisGUI.pro (depending on your system)

# Build
make

# Run
./FootAnalysisGUI
```

## Project Structure

```
foot/
├── FootAnalysisGUI.pro          # Qt project file
├── main.cpp                     # Application entry point
├── MainWindow.h                 # Main window header
├── MainWindow.cpp               # Main window implementation
└── foot-Function/               # Python analysis scripts
    ├── main.py                  # Main Python script
    ├── output_videos/           # Output directory (created at runtime)
    │   ├── output_video.avi     # Generated video output
    │   ├── data_output.json     # Generated JSON data
    │   └── data_output.csv      # Generated CSV data
    └── [other Python modules]
```

## Features

### New Features in This Version

1. **Tabbed Results Interface**:
   - **Summary Tab**: Overview and status messages
   - **Data Table Tab**: Displays CSV/JSON data with player statistics and team possession
   - **Video Output Tab**: Embedded video player with controls

2. **Automatic Data Loading**:
   - After Python analysis completes successfully, the application automatically:
     - Reads and displays CSV data in a table
     - Loads JSON data as fallback
     - Loads and prepares the output video for playback

3. **Video Playback**:
   - QMediaPlayer integration for native video playback
   - Play/Pause and Stop controls
   - Video displays in the embedded QVideoWidget

4. **Real-time Process Monitoring**:
   - Live stdout/stderr capture from Python process
   - Non-blocking UI (remains responsive during analysis)
   - Color-coded error messages

5. **Existing Features Maintained**:
   - Browse and select input video
   - Browse and select YOLO model
   - Asynchronous Python execution via QProcess
   - Status updates and error handling

## Usage

1. **Launch the Application**

2. **Select Input Video**:
   - Click "Browse..." next to "Input Video"
   - Select your football/soccer match video file

3. **Select YOLO Model**:
   - Click "Browse..." next to "YOLO Model"
   - Select the trained YOLO model file (usually `best.pt`)

4. **Start Analysis**:
   - Click "Start Analysis" button
   - Monitor progress in the "Analysis Log" section
   - UI remains responsive during processing

5. **View Results**:
   - **Data Table Tab**: View player statistics and possession percentages
   - **Video Output Tab**: Play the annotated video with detected players, ball tracking, and statistics

## Troubleshooting

### Build Errors

**Error: "Qt Multimedia module not found"**
- Solution: Ensure Qt Multimedia is installed with your Qt version
- In Qt Maintenance Tool, verify that Qt Multimedia modules are checked

**Error: "Unknown modules: multimedia multimediawidgets"**
- Solution: Your Qt version may not include these modules
- Install Qt 6.2 or later with Multimedia support

### Runtime Errors

**Error: "Python process failed to start"**
- Solution: Ensure Python is installed and accessible in system PATH
- Test by running `python --version` in command prompt/terminal

**Error: "Failed to load media"**
- Solution: Ensure Qt Multimedia plugins are installed
- Check that the video codec is supported by your system

**Error: "Output files not found"**
- Solution: Verify that Python analysis completed successfully
- Check the Analysis Log for Python errors
- Ensure `foot-Function` directory structure is intact

### Video Playback Issues

**Video shows black screen**
- Solution: Install codec pack (Windows) or gstreamer plugins (Linux)
- On Windows: Install K-Lite Codec Pack or similar
- On Linux: `sudo apt-get install gstreamer1.0-plugins-good gstreamer1.0-plugins-bad`

**No audio**
- Solution: This is expected - the analysis video typically has no audio
- If audio is expected, check QAudioOutput configuration

## Python Analysis Requirements

Ensure the Python environment has all dependencies installed:

```bash
cd foot-Function
pip install opencv-python numpy torch ultralytics
# Install other requirements as needed
```

## License

See repository license file for details.
