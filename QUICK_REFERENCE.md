# Quick Reference - Football Analysis Qt GUI

## File Overview

### Source Code (695 lines total)
- **main.cpp** (27 lines) - Application entry point
- **MainWindow.h** (158 lines) - Main window class declaration
- **MainWindow.cpp** (510 lines) - Main window implementation

### Build Configuration
- **CMakeLists.txt** - CMake build system
- **FootAnalysisGUI.pro** - qmake project file
- **build_qt_gui.sh** - Build automation script

### Documentation
- **README_GUI.md** - Architecture & build instructions
- **UI_DESIGN.md** - UI layout & interaction flows
- **USAGE_GUIDE.md** - Step-by-step usage examples
- **EXAMPLE_BUTTON_LOGIC.cpp** - Detailed code examples
- **IMPLEMENTATION_SUMMARY.md** - Complete implementation overview
- **QUICK_REFERENCE.md** - This file

## Quick Start

### Build & Run (3 steps)
```bash
# 1. Install Qt6
sudo apt-get install qt6-base-dev qt6-base-dev-tools

# 2. Build
mkdir build && cd build && cmake .. && cmake --build .

# 3. Run
./bin/FootAnalysisGUI
```

## Key Classes and Methods

### MainWindow
```cpp
// Constructor - initializes UI and QProcess
MainWindow(QWidget *parent = nullptr);

// File browsing
void onBrowseInputVideo();
void onBrowseModelFile();
void onBrowseOutputDir();

// Process control
void onRunAnalysis();        // Start analysis
void onStopAnalysis();       // Terminate process

// Output capture
void onProcessReadyReadStdout();   // Capture stdout
void onProcessReadyReadStderr();   // Capture stderr
void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

// Validation
bool validateInputs();       // Check all inputs before running
```

## Constants
```cpp
SEPARATOR_LENGTH = 60              // Length of separator line
PROCESS_START_TIMEOUT_MS = 5000    // Timeout to start process
PROCESS_TERMINATE_TIMEOUT_MS = 5000 // Timeout for graceful termination
PROCESS_KILL_TIMEOUT_MS = 3000     // Timeout before force kill
```

## UI Layout

```
MainWindow
├── Input Configuration (QGroupBox)
│   ├── Input Video (QLineEdit + Browse QPushButton)
│   ├── Model File (QLineEdit + Browse QPushButton)
│   ├── Output Directory (QLineEdit + Browse QPushButton)
│   └── Use Stubs (QCheckBox)
├── Process Output (QGroupBox)
│   └── Output Console (QTextEdit - read-only, monospace)
├── Control Buttons (QHBoxLayout)
│   ├── Run Analysis (QPushButton)
│   └── Stop (QPushButton)
└── Status Bar (QLabel)
```

## Signal/Slot Connections

### Browse Buttons
```cpp
m_browseInputBtn → onBrowseInputVideo()
m_browseModelBtn → onBrowseModelFile()
m_browseOutputBtn → onBrowseOutputDir()
```

### Control Buttons
```cpp
m_runBtn → onRunAnalysis()
m_stopBtn → onStopAnalysis()
```

### QProcess Signals
```cpp
readyReadStandardOutput → onProcessReadyReadStdout()
readyReadStandardError → onProcessReadyReadStderr()
finished(int, ExitStatus) → onProcessFinished(int, ExitStatus)
errorOccurred(ProcessError) → onProcessError(ProcessError)
stateChanged(ProcessState) → onProcessStateChanged(ProcessState)
```

## QProcess Execution Flow

### 1. Prepare Python Wrapper
```python
# Wrapper script template
import sys
sys.path.insert(0, '<foot-Function-path>')
from main import VideoAnalysisPipeline

pipeline = VideoAnalysisPipeline(
    input_video_path='<user-input>',
    model_path='<user-input>',
    output_dir='<user-input>',
    use_stubs=<user-choice>
)
pipeline.run()
```

### 2. Write to Temporary File
```cpp
QTemporaryFile tempFile("foot_analysis_wrapper_XXXXXX.py");
tempFile.open();
tempFile.write(wrapperScript);
```

### 3. Execute Process
```cpp
m_process->setWorkingDirectory(footFunctionPath);
m_process->start("python3", [tempScriptPath]);
```

### 4. Capture Output
```cpp
// stdout → black text in console
QString output = m_process->readAllStandardOutput();

// stderr → red text in console
QString error = m_process->readAllStandardError();
```

## Common Tasks

### Change Default Paths
Edit `MainWindow.cpp` constructor:
```cpp
m_inputVideoEdit->setText("/your/path/video.mp4");
m_modelFileEdit->setText("/your/path/model.pt");
m_outputDirEdit->setText("/your/output/directory");
```

### Change Timeout Values
Edit constants in `MainWindow.h`:
```cpp
static constexpr int PROCESS_START_TIMEOUT_MS = 10000; // 10 seconds
```

### Change Python Executable
Edit `onRunAnalysis()` in `MainWindow.cpp`:
```cpp
QString pythonExecutable = "/path/to/python3";
// or
QString pythonExecutable = "python";  // for Windows
```

### Add More Input Fields
1. Add widget in `MainWindow.h`:
   ```cpp
   QLineEdit* m_newFieldEdit;
   ```

2. Create widget in `createInputGroup()`:
   ```cpp
   m_newFieldEdit = new QLineEdit(this);
   layout->addWidget(m_newFieldEdit);
   ```

3. Use value in `onRunAnalysis()`:
   ```cpp
   QString newValue = m_newFieldEdit->text();
   ```

## Error Handling Checklist

### Before Process Start
- ✓ Input video file exists
- ✓ Model file exists
- ✓ Output directory accessible
- ✓ foot-Function directory exists
- ✓ main.py exists

### During Execution
- ✓ Process failed to start
- ✓ Process crashed
- ✓ Read/write errors
- ✓ Timeout errors

### After Completion
- ✓ Non-zero exit code
- ✓ Abnormal termination
- ✓ Output file verification

## Debugging Tips

### Enable Verbose Output
Add debug logging:
```cpp
qDebug() << "Process state:" << m_process->state();
qDebug() << "Working dir:" << m_process->workingDirectory();
qDebug() << "Arguments:" << arguments;
```

### Check Process State
```cpp
switch (m_process->state()) {
    case QProcess::NotRunning: // Not started or finished
    case QProcess::Starting:   // About to start
    case QProcess::Running:    // Currently executing
}
```

### Test Wrapper Script Manually
```bash
# Extract temp script and run manually
cat /tmp/foot_analysis_wrapper_*.py
python3 /tmp/foot_analysis_wrapper_*.py
```

## Build Configurations

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### Release Build
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### With Specific Qt Version
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64 ..
```

## Platform-Specific Notes

### Linux
- Python: `python3`
- Path separator: `/`
- Qt install: `apt-get install qt6-base-dev`

### macOS
- Python: `python3`
- Path separator: `/`
- Qt install: `brew install qt@6`

### Windows
- Python: `python` or `python3`
- Path separator: `\` (handled by QDir)
- Qt install: Download from qt.io

## Performance Tuning

### Speed Up Analysis
- ✓ Use cached stubs (checkbox)
- Process shorter video clips
- Use better GPU hardware

### Reduce Memory Usage
- Process in smaller chunks
- Lower video resolution
- Close other applications

## Testing Checklist

- [ ] Application builds successfully
- [ ] UI displays correctly
- [ ] Browse buttons open dialogs
- [ ] File paths validate correctly
- [ ] Run button starts process
- [ ] stdout appears in black
- [ ] stderr appears in red
- [ ] Stop button terminates process
- [ ] Status bar updates
- [ ] Success dialog appears
- [ ] Output files created
- [ ] Multiple instances don't conflict

## Integration Points

### With foot-Function
- No modifications required
- Uses `VideoAnalysisPipeline` class
- Passes parameters via constructor
- Captures output via stdout/stderr

### With File System
- Reads: video files, model files
- Writes: output video, JSON, CSV
- Temp: Python wrapper scripts

### With Qt Framework
- QProcess: External execution
- QFileDialog: File browsing
- QMessageBox: User dialogs
- QTextEdit: Output display

## Troubleshooting Quick Fixes

| Problem | Solution |
|---------|----------|
| Won't build | Install Qt6: `apt-get install qt6-base-dev` |
| Can't find python3 | Add to PATH or use absolute path |
| foot-Function not found | Place app in same dir as foot-Function |
| Model file missing | Download and place in models/ |
| Process hangs | Click Stop button, check Python dependencies |
| No output shown | Check stdout/stderr capture is working |

## Additional Resources

- Qt6 Documentation: https://doc.qt.io/qt-6/
- QProcess Class: https://doc.qt.io/qt-6/qprocess.html
- CMake Documentation: https://cmake.org/documentation/
- foot-Function: See foot-Function/main.py

## Version Information

- **Qt Version**: Qt 6.x required
- **C++ Standard**: C++17
- **CMake Version**: 3.16 minimum
- **Python Version**: Python 3.7+ (for foot-Function)

## License

This Qt GUI wrapper follows the same license as the foot-Function project.

---

For detailed information, see:
- Architecture: README_GUI.md
- UI Design: UI_DESIGN.md
- Usage Examples: USAGE_GUIDE.md
- Code Examples: EXAMPLE_BUTTON_LOGIC.cpp
- Full Summary: IMPLEMENTATION_SUMMARY.md
