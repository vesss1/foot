# Quick Start Guide - Video Data Viewer

## For Users (Windows Example)

### Step 1: Install Prerequisites

1. **Install Qt 6.10.1**
   - Download Qt Online Installer from https://www.qt.io/download
   - During installation, select:
     - Qt 6.10.1
     - MSVC 2019 64-bit component
     - Qt Creator

2. **Install OpenCV 4.x**
   - Download from https://opencv.org/releases/ (e.g., OpenCV 4.10.1)
   - Extract to `C:\opencv`
   - Add to System PATH: `C:\opencv\build\x64\vc16\bin`

### Step 2: Build the Application

#### Option A: Using Qt Creator (Easiest)
1. Open Qt Creator
2. File → Open File or Project → Select `VideoDataViewer.pro`
3. Configure with Desktop Qt 6.10.1 MSVC kit
4. Click the Build button (Hammer icon)
5. Click Run button (Green play icon)

#### Option B: Using Command Line
```cmd
cd C:\path\to\foot
qmake VideoDataViewer.pro
nmake  # or mingw32-make if using MinGW
```

### Step 3: Prepare Your Data

Place your files in: `foot-Function\output_videos\`
- Required: `output.avi`
- Optional: `output_data.csv` OR `output_data.json`

### Step 4: Run the Application

The viewer will automatically:
1. Load the video from `foot-Function/output_videos/output.avi`
2. Find and load CSV or JSON data file
3. Start playing the video

## Application Interface

```
┌─────────────────────────────────────────────────────────┐
│  Video and Data Viewer - Qt 6.10.1                      │
├─────────────────────────────────────────────────────────┤
│ ┌─ Video Playback ───────────────────────────────────┐ │
│ │                                                     │ │
│ │                                                     │ │
│ │              [Video Display Area]                  │ │
│ │                                                     │ │
│ │                                                     │ │
│ └─────────────────────────────────────────────────────┘ │
│ │ [Pause] [Reset]              Frame: 45 / 90       │ │
│ └─────────────────────────────────────────────────────┘ │
│                                                         │
│ ┌─ Data Display ────────────────────────────────────┐  │
│ │ Display Mode: [Table View ▼]                      │  │
│ │ ┌───────────────────────────────────────────────┐ │  │
│ │ │ Frame │ Time  │ Objects │ Ball_X │ Ball_Y    │ │  │
│ │ ├───────┼───────┼─────────┼────────┼───────────┤ │  │
│ │ │   0   │ 0.00  │    5    │  320   │   240     │ │  │
│ │ │   1   │ 0.03  │    5    │  325   │   238     │ │  │
│ │ │   2   │ 0.07  │    6    │  330   │   235     │ │  │
│ │ └───────────────────────────────────────────────┘ │  │
│ └───────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

## Controls

- **Pause Button**: Pause/Resume video playback
- **Reset Button**: Go back to frame 0
- **Display Mode Dropdown**: Switch between:
  - **Table View**: Shows data in formatted table (best for CSV)
  - **Raw Text**: Shows original file content

## Supported Formats

### Video Files
- **AVI** (XVID codec) - Recommended
- Should work with MP4, MOV if OpenCV has codec support

### Data Files
- **CSV**: Comma-separated with headers in first row
- **JSON**: Arrays of objects or single object with nested data

## Creating Your Own Data

### Example CSV
```csv
Frame,Time,Objects_Detected,Ball_Position_X,Ball_Position_Y
0,0.00,5,320,240
1,0.03,5,325,238
2,0.07,6,330,235
```

### Example JSON
```json
[
  {
    "frame": 0,
    "time": 0.00,
    "objects": 5,
    "ball_x": 320,
    "ball_y": 240
  },
  {
    "frame": 1,
    "time": 0.03,
    "objects": 5,
    "ball_x": 325,
    "ball_y": 238
  }
]
```

## Testing with Sample Data

Run the included Python script to generate test files:

```bash
python create_test_video.py
```

This creates:
- `foot-Function/output_videos/output.avi` (3 seconds, 30 fps)
- `foot-Function/output_videos/output_data.csv` (21 rows)
- `foot-Function/output_videos/output_data.json` (nested structure)

## Common Issues

### "Video file not found"
- Ensure `output.avi` exists in `foot-Function/output_videos/`
- Check file path is correct relative to executable
- Try absolute path if needed

### "Failed to open video"
- Install OpenCV with codec support
- Try different video codec (XVID recommended)
- Verify video file is not corrupted

### Build fails with "opencv.hpp not found"
- Verify OpenCV installed correctly
- Update paths in `VideoDataViewer.pro`:
  ```qmake
  INCLUDEPATH += C:/opencv/build/include
  ```

### Application crashes on start
- Ensure OpenCV DLLs are in PATH
- Check Qt libraries are accessible
- Run from Qt Creator to see detailed errors

## Linux Quick Start

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install qt6-base-dev libopencv-dev

# Build
cd /path/to/foot
qmake VideoDataViewer.pro
make

# Run
./VideoDataViewer
```

## macOS Quick Start

```bash
# Install dependencies
brew install qt opencv

# Build
cd /path/to/foot
qmake VideoDataViewer.pro
make

# Run
./VideoDataViewer
```

## Features Overview

✅ Real-time video playback with OpenCV
✅ CSV and JSON data display
✅ Table view with sortable columns
✅ Raw text view for debugging
✅ Pause/Resume/Reset controls
✅ Frame counter display
✅ Resizable video/data sections
✅ Auto-detection of data files
✅ Maintains aspect ratio for video
✅ Smooth frame rendering

## Advanced Configuration

### Change Default Video Path

Edit `VideoDataViewer.cpp` around line 47:

```cpp
QString outputDir = QDir::currentPath() + "/foot-Function/output_videos";
// Change to:
QString outputDir = "C:/MyVideos/output";
```

### Adjust Video FPS

The viewer automatically uses the video's FPS, but you can override:

```cpp
// In VideoDataViewer.cpp, loadVideo() function
fps = 30.0;  // Force 30 FPS
```

### Add Command-Line Arguments

Modify `viewer_main.cpp`:

```cpp
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    VideoDataViewer viewer;
    
    // Load custom video if provided
    if (argc > 1) {
        viewer.loadVideo(QString(argv[1]));
    }
    
    viewer.show();
    return app.exec();
}
```

## Performance Tips

- For large videos (>1GB), consider streaming or frame caching
- Reduce frame rate if playback is choppy
- Use XVID codec for best compatibility
- CSV files are faster to parse than JSON

## Project Structure

```
foot/
├── VideoDataViewer.h          # Class definition
├── VideoDataViewer.cpp        # Implementation
├── viewer_main.cpp            # Entry point
├── VideoDataViewer.pro        # Qt project (qmake)
├── CMakeLists_Viewer.txt      # CMake alternative
├── VIEWER_README.md           # Full documentation
├── USAGE_GUIDE.md             # This file
├── create_test_video.py       # Test data generator
└── foot-Function/
    └── output_videos/
        ├── output.avi         # Your video
        ├── output_data.csv    # Your CSV data
        └── output_data.json   # Your JSON data
```

## Integration with Existing Project

The viewer is separate from the main `FootAnalysisGUI` application:

- **FootAnalysisGUI.pro** → Analysis application (existing)
- **VideoDataViewer.pro** → Viewer application (new)

Both can coexist and be built independently.

## Next Steps

1. Run the test video generator
2. Build the viewer
3. Test with sample data
4. Replace with your own video and data
5. Customize as needed

## Support

For detailed API documentation, see `VIEWER_README.md`
For Qt questions: https://doc.qt.io/
For OpenCV questions: https://docs.opencv.org/
