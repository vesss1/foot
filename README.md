# Football Analysis Project

This repository contains a football video analysis system with two components:

## 1. foot-Function (Python Pipeline)
Python-based football video analysis using YOLO object detection, tracking, and statistical analysis.

**Location**: `foot-Function/`

**Features**:
- YOLO-based player and ball detection
- Multi-object tracking
- Team assignment via jersey color analysis
- Camera movement estimation
- Speed and distance calculations
- Ball possession tracking
- Annotated video output

**Usage**: See `foot-Function/main.py`

## 2. Qt Creator GUI (C++ / Qt 6)
Modern graphical user interface for the foot-Function pipeline.

**Location**: Root directory

**Key Files**:
- `main.cpp`, `MainWindow.h`, `MainWindow.cpp` - Qt application
- `CMakeLists.txt`, `FootAnalysisGUI.pro` - Build configuration
- `README_GUI.md` - Architecture documentation
- `USAGE_GUIDE.md` - User guide with examples
- `QUICK_REFERENCE.md` - Developer reference

**Features**:
- ✅ **Zero modifications to foot-Function** - Uses QProcess for integration
- ✅ **User-friendly interface** - File dialogs, configuration forms
- ✅ **Real-time output** - Live stdout/stderr capture with color coding
- ✅ **Argument passing** - UI parameters → Python script
- ✅ **Error handling** - Comprehensive validation and error reporting
- ✅ **Cross-platform** - Works on Linux, macOS, Windows
- ✅ **Production quality** - Named constants, QTemporaryFile, robust path handling

## Quick Start

### Prerequisites
```bash
# Qt6 (Ubuntu/Debian)
sudo apt-get install qt6-base-dev qt6-base-dev-tools

# Python dependencies
pip install opencv-python numpy ultralytics supervision
```

### Build Qt GUI
```bash
# CMake
mkdir build && cd build
cmake ..
cmake --build .
./bin/FootAnalysisGUI

# Or use Qt Creator IDE
# Open FootAnalysisGUI.pro or CMakeLists.txt
```

### Using the GUI
1. Launch FootAnalysisGUI
2. Browse to select:
   - Input video file
   - YOLO model file
   - Output directory
3. Check "Use Cached Stubs" for faster processing
4. Click "Run Analysis"
5. Monitor progress in output console
6. Check output directory for results

## Documentation

### For Users
- **[USAGE_GUIDE.md](USAGE_GUIDE.md)** - Complete usage guide with examples
- **[UI_DESIGN.md](UI_DESIGN.md)** - UI layout and interaction flows

### For Developers
- **[README_GUI.md](README_GUI.md)** - Architecture and build instructions
- **[EXAMPLE_BUTTON_LOGIC.cpp](EXAMPLE_BUTTON_LOGIC.cpp)** - Detailed code examples
- **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - Quick reference guide
- **[IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md)** - Complete implementation overview

## Architecture

```
FootAnalysisGUI (Qt6 C++)
    ↓ QProcess
    ↓ Temporary Python Wrapper Script
    ↓ 
foot-Function (Python)
    ↓ VideoAnalysisPipeline
    ↓
Output Files (video, JSON, CSV)
```

**Key Design**: The Qt GUI creates a temporary Python script that imports and executes the VideoAnalysisPipeline class from foot-Function. This ensures **zero modifications** to the original Python code.

## Project Structure

```
foot/
├── foot-Function/              # Python pipeline (UNCHANGED)
│   ├── main.py
│   ├── trackers/
│   ├── team_assigner/
│   └── ...
├── main.cpp                    # Qt application entry
├── MainWindow.h                # Main window declaration
├── MainWindow.cpp              # Main window implementation
├── CMakeLists.txt              # CMake build
├── FootAnalysisGUI.pro         # qmake build
├── build_qt_gui.sh             # Build script
├── README.md                   # This file
├── README_GUI.md               # Qt GUI documentation
├── USAGE_GUIDE.md              # User guide
├── QUICK_REFERENCE.md          # Developer reference
├── UI_DESIGN.md                # UI design documentation
├── EXAMPLE_BUTTON_LOGIC.cpp    # Code examples
└── IMPLEMENTATION_SUMMARY.md   # Implementation details
```

## Requirements

### Qt GUI Application
- Qt 6.x
- C++17 compiler
- CMake 3.16+ or qmake

### foot-Function (Python)
- Python 3.7+
- OpenCV (cv2)
- NumPy
- Ultralytics YOLO
- Supervision

## Features Implemented

### QProcess Integration
- ✅ External Python process execution
- ✅ Working directory management
- ✅ Command-line argument passing
- ✅ Process state monitoring
- ✅ Graceful termination with timeout
- ✅ Force kill fallback

### Output Capture
- ✅ Real-time stdout capture (black text)
- ✅ Real-time stderr capture (red text)
- ✅ Auto-scrolling output console
- ✅ Monospace font for readability
- ✅ Line-by-line display

### User Interface
- ✅ Input video file selection (Browse dialog)
- ✅ Model file selection (Browse dialog)
- ✅ Output directory selection (Browse dialog)
- ✅ Use stubs checkbox
- ✅ Run/Stop buttons
- ✅ Status bar with live updates
- ✅ Success/error dialogs

### Error Handling
- ✅ Input validation (file existence)
- ✅ Directory creation
- ✅ Process start failure detection
- ✅ Process crash detection
- ✅ Exit code monitoring
- ✅ User-friendly error messages

### Code Quality
- ✅ Cross-platform path handling (QDir)
- ✅ Unique temporary files (QTemporaryFile)
- ✅ Named constants for magic numbers
- ✅ Robust path resolution (applicationDirPath)
- ✅ Modern C++17 features
- ✅ Qt6 best practices
- ✅ Comprehensive documentation

## Output Files

After successful analysis, check the output directory for:

- **output_video.avi** - Annotated video with tracking overlays
- **data_output.json** - Frame-by-frame tracking data
- **data_output.csv** - Tabular tracking data

## Troubleshooting

### Build Issues
- **Qt6 not found**: Install qt6-base-dev package
- **CMake not found**: Install cmake package

### Runtime Issues
- **Process won't start**: Check Python 3 is installed and in PATH
- **foot-Function not found**: Ensure app is in same directory
- **Model not found**: Download YOLO model to models/ directory

### Analysis Issues
- **No players detected**: Ensure video shows football gameplay
- **Process hangs**: Click Stop button, check Python dependencies

For detailed troubleshooting, see [USAGE_GUIDE.md](USAGE_GUIDE.md).

## Contributing

When contributing to this Qt GUI:
- **DO NOT** modify files in foot-Function directory
- Follow Qt coding conventions
- Ensure cross-platform compatibility
- Update documentation for new features
- Test on multiple platforms if possible

## License

This Qt GUI wrapper follows the same license as the foot-Function project.

## Credits

- **foot-Function**: Python-based football analysis pipeline
- **Qt GUI**: Complete C++ / Qt 6 graphical interface

## Additional Resources

- [Qt6 Documentation](https://doc.qt.io/qt-6/)
- [QProcess Documentation](https://doc.qt.io/qt-6/qprocess.html)
- [CMake Documentation](https://cmake.org/documentation/)
- [YOLO by Ultralytics](https://ultralytics.com/)

