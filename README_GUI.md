# Football Analysis - Qt GUI

This Qt Creator (C++ / Qt 6) application provides a graphical user interface for the existing `foot-Function` Python-based football video analysis pipeline.

## Features

- **QProcess Integration**: Executes the existing Python scripts without modifying foot-Function code
- **Argument Passing**: Configure input video, model file, output directory, and stub usage from the GUI
- **Real-time Output**: Captures and displays stdout/stderr from the Python process in real-time
- **Error Handling**: Comprehensive error checking and user-friendly error messages
- **Process Control**: Start/Stop buttons for process management

## Requirements

### Python Dependencies (for foot-Function)
The foot-Function Python pipeline requires:
- Python 3.7+
- OpenCV (cv2)
- NumPy
- YOLO model dependencies

### Qt Application Requirements
- Qt 6.x
- C++17 compiler
- CMake 3.16+ or qmake

## Building the Application

### Using CMake (Recommended)

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./bin/FootAnalysisGUI
```

### Using qmake

```bash
# Generate Makefile
qmake FootAnalysisGUI.pro

# Build
make

# Run
./FootAnalysisGUI
```

### Using Qt Creator

1. Open `FootAnalysisGUI.pro` or `CMakeLists.txt` in Qt Creator
2. Configure the project for your Qt Kit
3. Build the project (Ctrl+B or Cmd+B)
4. Run the project (Ctrl+R or Cmd+R)

## Usage

1. **Launch the Application**: Start the FootAnalysisGUI executable

2. **Configure Input Parameters**:
   - **Input Video**: Browse to select your football video file (mp4, avi, mov, mkv)
   - **Model File**: Browse to select the YOLO model file (.pt or .pth)
   - **Output Directory**: Browse to select where output files will be saved
   - **Use Cached Stubs**: Check to use cached processing data for faster execution

3. **Run Analysis**: Click the "Run Analysis" button to start processing

4. **Monitor Progress**: Watch the output console for real-time logs from the Python script

5. **View Results**: Once complete, check the output directory for:
   - `output_video.avi` - Annotated video with tracking overlays
   - `data_output.json` - JSON data with tracking information
   - `data_output.csv` - CSV data with tracking information

## Architecture

### Component Overview

- **main.cpp**: Application entry point, initializes QApplication
- **MainWindow.h/cpp**: Main GUI window implementation
  - Input configuration widgets
  - Output console for real-time logs
  - QProcess integration for Python script execution
  - Signal/slot connections for process events

### Process Flow

1. User configures parameters in the GUI
2. Application validates inputs (file existence, directory access)
3. Creates a temporary Python wrapper script with user parameters
4. Launches Python process via QProcess
5. Captures stdout/stderr and displays in real-time
6. Reports completion status and errors to the user

### Python Integration

The application creates a temporary Python wrapper script that:
- Imports the `VideoAnalysisPipeline` class from `foot-Function/main.py`
- Instantiates the pipeline with user-provided parameters
- Executes the analysis
- Reports results back to the Qt application

This approach ensures **zero modifications** to the existing foot-Function code.

## Code Structure

```
foot/
├── foot-Function/          # Original Python pipeline (UNCHANGED)
│   ├── main.py
│   ├── trackers/
│   ├── team_assigner/
│   ├── camera_movement_estimator/
│   └── ...
├── MainWindow.h            # Qt GUI header
├── MainWindow.cpp          # Qt GUI implementation
├── main.cpp                # Qt application entry point
├── CMakeLists.txt          # CMake build configuration
├── FootAnalysisGUI.pro     # qmake project file
└── README_GUI.md           # This file
```

## Implementation Details

### QProcess Integration

The application uses Qt's `QProcess` class to execute Python scripts:

```cpp
m_process = new QProcess(this);
m_process->setWorkingDirectory(m_footFunctionPath);
m_process->start("python3", arguments);
```

### Signal/Slot Connections

```cpp
connect(m_process, &QProcess::readyReadStandardOutput,
        this, &MainWindow::onProcessReadyReadStdout);
connect(m_process, &QProcess::readyReadStandardError,
        this, &MainWindow::onProcessReadyReadStderr);
connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &MainWindow::onProcessFinished);
```

### Output Capture

Stdout and stderr are captured separately and displayed with different formatting:

```cpp
void MainWindow::onProcessReadyReadStdout() {
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());
    appendOutput(output, false);  // Black text
}

void MainWindow::onProcessReadyReadStderr() {
    QString output = QString::fromUtf8(m_process->readAllStandardError());
    appendOutput(output, true);   // Red text
}
```

## Troubleshooting

### "Failed to start process"
- Ensure Python 3 is installed and accessible in PATH
- Try running `python3 --version` in terminal

### "main.py not found"
- Ensure the application is in the same directory as foot-Function
- Check that foot-Function directory structure is intact

### "Model file does not exist"
- Ensure you have downloaded the YOLO model file
- Place it in `foot-Function/models/` directory

### "Input video file does not exist"
- Verify the video file path is correct
- Ensure video file format is supported (mp4, avi, mov, mkv)

## License

This GUI wrapper follows the same license as the foot-Function project.

## Contributing

When contributing to this Qt GUI:
- Do NOT modify any files in the `foot-Function` directory
- Follow Qt coding conventions
- Ensure all QProcess error cases are handled
- Update this README for any new features
