# Football Analysis Qt GUI - Usage Guide

## Quick Start

### Prerequisites Installation

#### Ubuntu/Debian
```bash
# Install Qt6 development tools
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-base-dev-tools

# Install C++ build tools
sudo apt-get install build-essential cmake

# Install Python3 (if not already installed)
sudo apt-get install python3 python3-pip

# Install Python dependencies for foot-Function
cd foot-Function
pip3 install opencv-python numpy ultralytics supervision
```

#### macOS
```bash
# Install Qt6 via Homebrew
brew install qt@6

# Install Python3 (if not already installed)
brew install python3

# Install Python dependencies
cd foot-Function
pip3 install opencv-python numpy ultralytics supervision
```

#### Windows
1. Download and install Qt Creator from https://www.qt.io/download-qt-installer
2. Install Python 3 from https://www.python.org/downloads/
3. Install Python dependencies:
   ```cmd
   cd foot-Function
   pip install opencv-python numpy ultralytics supervision
   ```

### Building the Application

#### Method 1: Using CMake (Recommended)
```bash
# Navigate to project root
cd /path/to/foot

# Create build directory
mkdir build
cd build

# Configure project
cmake ..

# Build
cmake --build .

# Run
./bin/FootAnalysisGUI
```

#### Method 2: Using qmake
```bash
# Navigate to project root
cd /path/to/foot

# Generate Makefile
qmake FootAnalysisGUI.pro

# Build
make

# Run
./FootAnalysisGUI
```

#### Method 3: Using Qt Creator IDE
1. Open Qt Creator
2. File → Open File or Project
3. Select `FootAnalysisGUI.pro` or `CMakeLists.txt`
4. Configure your Qt Kit (Qt 6.x)
5. Click "Configure Project"
6. Build → Build Project (Ctrl+B / Cmd+B)
7. Run → Run (Ctrl+R / Cmd+R)

## Usage Examples

### Example 1: Basic Video Analysis

**Scenario:** Analyze a football match video with default settings

**Steps:**
1. Launch FootAnalysisGUI
2. Input Video: Browse to `foot-Function/input_videos/08fd33_4.mp4`
3. Model File: Browse to `foot-Function/models/best.pt`
4. Output Directory: `foot-Function/output_videos` (default)
5. Check "Use Cached Stubs" for faster processing
6. Click "Run Analysis"
7. Wait for completion (monitor output console)
8. Check output directory for results

**Expected Output:**
- `output_videos/output_video.avi` - Annotated video
- `output_videos/data_output.json` - Tracking data (JSON)
- `output_videos/data_output.csv` - Tracking data (CSV)

### Example 2: Processing Without Stubs (Full Analysis)

**Scenario:** Run complete analysis without using cached data

**Steps:**
1. Launch FootAnalysisGUI
2. Configure input video and model as above
3. **Uncheck** "Use Cached Stubs"
4. Click "Run Analysis"
5. Note: Processing will take longer

**Use Case:** 
- First-time analysis of a new video
- When you want fresh detection/tracking data
- When stub files are outdated or corrupt

### Example 3: Analyzing Multiple Videos

**Scenario:** Process several videos in sequence

**Steps:**
For each video:
1. Click "Browse..." next to Input Video
2. Select video file
3. Optionally change output directory (e.g., `output_videos/game1`)
4. Click "Run Analysis"
5. Wait for completion
6. Repeat for next video

**Tip:** Each video gets its own output folder to avoid overwriting results.

### Example 4: Stopping a Long-Running Process

**Scenario:** Cancel analysis that's taking too long

**Steps:**
1. While analysis is running, click "Stop" button
2. Wait for graceful shutdown (up to 5 seconds)
3. If needed, process will be force-killed
4. Check output console for termination message

**Use Case:**
- Wrong video selected
- Process appears stuck
- Need to adjust parameters

### Example 5: Troubleshooting Failed Analysis

**Scenario:** Analysis fails with error

**Investigation Steps:**
1. Check output console for error messages (red text)
2. Common issues:
   - **"Model file not found"**: Ensure model file exists and path is correct
   - **"Cannot open video file"**: Video file may be corrupt or unsupported format
   - **"No frames read from video"**: Video file is empty or invalid
   - **"Failed to initialize tracker"**: Model file is corrupt or incompatible
   - **"No players detected"**: Video doesn't contain football gameplay

**Solutions:**
- Verify all file paths are correct
- Ensure video is a supported format (mp4, avi, mov, mkv)
- Check that model file is valid YOLO format (.pt)
- Ensure Python dependencies are installed

### Example 6: Custom Output Directory

**Scenario:** Save outputs to a custom location

**Steps:**
1. Launch FootAnalysisGUI
2. Configure input video and model
3. Click "Browse..." next to Output Directory
4. Create or select a new directory (e.g., `/home/user/football_results`)
5. Click "Run Analysis"
6. Results will be saved to custom directory

**Use Case:**
- Organizing results by date, team, or match
- Saving to external drive or network location

## Command-Line Output Explained

### Startup Messages
```
==========================================================
Starting Football Analysis...
Time: 2026-01-01 15:18:00
==========================================================
Input Video: /path/to/video.mp4
Model File: /path/to/best.pt
Output Directory: /path/to/output_videos
Use Stubs: Yes

Command: python3 /tmp/foot_analysis_wrapper.py
Working Directory: /path/to/foot-Function
==========================================================
```
**Meaning:** Configuration summary before starting analysis

### Pipeline Progress Messages
```
2026-01-01 15:18:01 - INFO - Reading video: input_videos/08fd33_4.mp4
2026-01-01 15:18:05 - INFO - Successfully read 300 frames
```
**Meaning:** Video loaded, 300 frames to process

```
2026-01-01 15:18:06 - INFO - Initializing tracker
2026-01-01 15:18:10 - INFO - Object tracking complete
```
**Meaning:** YOLO detection and tracking phase completed

```
2026-01-01 15:18:15 - INFO - Camera movement processing complete
```
**Meaning:** Camera motion analysis done

```
2026-01-01 15:18:16 - INFO - Speed and distance calculation complete
```
**Meaning:** Player speed/distance metrics computed

```
2026-01-01 15:18:18 - INFO - Team assignment complete
```
**Meaning:** Players assigned to teams based on jersey color

```
2026-01-01 15:18:25 - INFO - Annotation drawing complete
```
**Meaning:** Visual overlays added to video frames

```
2026-01-01 15:18:35 - INFO - Output video saved successfully (145.67 MB)
```
**Meaning:** Final video written to disk

### Completion Messages
```
==========================================================
Process completed successfully!
==========================================================
```
**Meaning:** Analysis finished without errors

### Error Messages (in red)
```
ERROR: Model file not found: /path/to/model.pt
```
**Meaning:** Model file path is incorrect or file doesn't exist

```
ERROR: Cannot open video file: /path/to/video.mp4
```
**Meaning:** Video file is missing, corrupt, or unsupported format

## Performance Tips

### Speeding Up Analysis

1. **Use Stubs**: Check "Use Cached Stubs" for previously processed videos
   - First run: 30-60 minutes (depending on video length)
   - Subsequent runs with stubs: 5-10 minutes

2. **Shorter Videos**: Process shorter clips for testing
   - Trim video to 30-60 seconds for quick validation

3. **Hardware**: Better GPU = faster processing
   - YOLO detection is GPU-accelerated when available
   - CPU-only processing is significantly slower

### Resource Usage

**Typical Resource Requirements:**
- **RAM**: 4-8 GB (depends on video resolution/length)
- **Disk Space**: 
  - Input video: varies
  - Output video: ~100-200 MB for 300 frames
  - JSON/CSV: ~1-5 MB
- **Processing Time**: 
  - With GPU: 10-20 minutes for 300 frames
  - CPU only: 30-60 minutes for 300 frames
  - With stubs: 5-10 minutes

## Output Files Explained

### output_video.avi
- **Format**: AVI video file
- **Content**: Original video with annotations:
  - Bounding boxes around players and ball
  - Player IDs and team colors
  - Speed and distance metrics overlay
  - Ball possession indicator
  - Camera movement visualization
- **Usage**: Visual review of tracking accuracy

### data_output.json
- **Format**: JSON
- **Content**: Frame-by-frame tracking data:
  ```json
  {
    "players": {
      "0": {  // Frame number
        "1": {  // Player ID
          "bbox": [x1, y1, x2, y2],
          "team": 1,
          "speed": 15.3,
          "distance": 25.5,
          "has_ball": false
        }
      }
    },
    "ball": { ... },
    "referees": { ... }
  }
  ```
- **Usage**: Programmatic analysis, custom visualizations

### data_output.csv
- **Format**: CSV (Comma-Separated Values)
- **Content**: Tabular tracking data
- **Columns**: frame, player_id, team, x1, y1, x2, y2, speed, distance, has_ball
- **Usage**: Spreadsheet analysis, statistical studies

## Keyboard Shortcuts

When using Qt Creator IDE:
- **Ctrl+B / Cmd+B**: Build project
- **Ctrl+R / Cmd+R**: Run application
- **Ctrl+Q / Cmd+Q**: Quit application

Within the application:
- Currently no keyboard shortcuts defined
- (Can be added in future versions)

## Troubleshooting

### Build Issues

**Problem:** "Qt6 not found"
**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install qt6-base-dev qt6-base-dev-tools

# macOS
brew install qt@6
export CMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
```

**Problem:** "cmake: command not found"
**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake
```

### Runtime Issues

**Problem:** "Failed to start process"
**Solution:** 
- Check Python3 is installed: `python3 --version`
- Ensure Python3 is in PATH
- Try absolute path to python3 in code if needed

**Problem:** "foot-Function directory not found"
**Solution:**
- Ensure FootAnalysisGUI executable is in same directory as foot-Function
- Or modify `m_footFunctionPath` in MainWindow constructor

**Problem:** "Model file does not exist"
**Solution:**
- Download YOLO model and place in `foot-Function/models/`
- Or specify full path to existing model file

### Analysis Issues

**Problem:** "No players detected in first frame"
**Solution:**
- Ensure video starts with visible players on field
- Try different video or trim beginning

**Problem:** Process hangs or is very slow
**Solution:**
- Check if GPU is being used (if available)
- Reduce video resolution
- Process shorter clips
- Ensure sufficient RAM available

## Advanced Configuration

### Modifying Default Paths

Edit `MainWindow.cpp` constructor:
```cpp
// Change default paths
m_inputVideoEdit->setText("/your/custom/path/video.mp4");
m_modelFileEdit->setText("/your/custom/path/model.pt");
m_outputDirEdit->setText("/your/custom/output/path");
```

### Changing foot-Function Location

Edit `MainWindow.cpp` constructor:
```cpp
// Specify absolute path to foot-Function
m_footFunctionPath = "/absolute/path/to/foot-Function";
```

### Using Python Virtual Environment

If foot-Function uses a virtual environment, modify the Python executable:

Edit `MainWindow.cpp` in `onRunAnalysis()`:
```cpp
// Use venv Python
QString pythonExecutable = "/path/to/venv/bin/python3";
```

## Support

For issues with:
- **Qt GUI**: Check this documentation and Qt documentation
- **foot-Function**: Refer to foot-Function documentation
- **Python dependencies**: Check Python package documentation

## Next Steps

After successful analysis:
1. Review output video to verify tracking accuracy
2. Analyze JSON/CSV data for insights
3. Adjust parameters if needed (model, stubs, etc.)
4. Process additional videos
5. Integrate results into your workflow

## Additional Resources

- Qt6 Documentation: https://doc.qt.io/qt-6/
- QProcess Documentation: https://doc.qt.io/qt-6/qprocess.html
- OpenCV Python: https://opencv.org/
- YOLO: https://ultralytics.com/
