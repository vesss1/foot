# Qt GUI Application Guide

## Overview

The Football Analysis Qt GUI application (`qt_main.py`) provides an intuitive graphical interface for analyzing football match videos. This guide will help you get started with the application.

## Prerequisites

Before running the Qt GUI application, ensure you have:

1. **Python 3.7 or higher** installed
2. **All required dependencies** installed (see Installation section)
3. **YOLO model file** (`models/best.pt`) downloaded and placed in the models directory
4. **Input video file** ready for analysis

## Installation

### Step 1: Install Dependencies

```bash
# Install all required packages using requirements.txt
pip install -r requirements.txt

# Or install packages individually:
pip install PyQt5 ultralytics supervision opencv-python numpy matplotlib pandas scikit-learn
```

### Step 2: Download Model

Download the trained YOLO v5 model from:
- [Trained Yolo v5](https://drive.google.com/file/d/1DC2kCygbBWUKheQ_9cFziCsYVSRw6axK/view?usp=sharing)

Place the downloaded model file in the `models/` directory as `best.pt`.

### Step 3: Prepare Input Video

Place your football match video in the `input_videos/` directory, or have it ready in any accessible location.

## Opening Project in Qt Creator (Optional)

If you prefer to use Qt Creator IDE for development:

1. Open Qt Creator
2. Select "File" → "Open File or Project..."
3. Navigate to the project directory
4. Select the `foot.pro` file
5. Click "Open"

Qt Creator will load the project with all Python files organized and accessible. You can then:
- Browse and edit Python source files
- Use Qt Creator's search and navigation features
- View the project structure in the file tree
- Still run the application using Python from terminal or Qt Creator's terminal

**Note**: Qt Creator is primarily a C++/Qt IDE, but it can open Python projects as generic projects. For running the Python application, continue to use the commands below.

## Launching the Application

```bash
python qt_main.py
```

The application window will open with two main sections:
- **Left Panel**: Controls, settings, and log
- **Right Panel**: Video player for preview and playback

## Using the Application

### 1. Select Input Video

1. Click the **"Browse..."** button next to "No video selected"
2. Navigate to your video file (supports .mp4, .avi, .mov)
3. Select the file and click "Open"
4. The video preview will load in the player on the right

### 2. Configure Settings (Optional)

**Use cached data:**
- Check this option to use previously computed tracking and camera movement data
- Significantly speeds up re-analysis of the same video
- Unchecked: Full analysis from scratch (slower but always fresh)

**Model selection:**
- Default: `models/best.pt`
- Custom: Select "Custom..." to browse for a different YOLO model

**Output path:**
- Click **"Change..."** to specify a custom output location
- Default: `output_videos/output_video.avi`

### 3. Start Analysis

1. Click the **"Start Analysis"** button (green button)
2. The analysis process will begin in the background
3. Monitor progress through:
   - **Progress Bar**: Shows completion percentage (0-100%)
   - **Status Label**: Shows current processing step
   - **Analysis Log**: Detailed log of all operations

### 4. During Analysis

- The application remains responsive during processing
- You can read the log to see detailed progress
- Click **"Stop"** to cancel analysis at any time
- Processing stages include:
  1. Reading video (5%)
  2. Initializing tracker (10%)
  3. Detecting and tracking objects (15%)
  4. Calculating positions (25%)
  5. Estimating camera movement (30%)
  6. Applying perspective transformation (40%)
  7. Interpolating ball positions (45%)
  8. Calculating speed and distance (50%)
  9. Assigning teams (55%)
  10. Analyzing ball possession (60%)
  11. Generating annotated frames (70%)
  12. Saving output video (90%)
  13. Complete (100%)

### 5. View Results

Once analysis is complete:
1. The output video automatically loads in the player
2. Use the **playback controls**:
   - **Play/Pause**: Start or pause video playback
   - **Slider**: Drag to jump to any frame
   - **Frame counter**: Shows current frame number

### 6. Review Output

The analyzed video includes:
- **Player tracking**: Colored ellipses around players (team colors)
- **Player IDs**: Numbers displayed on each player
- **Ball tracking**: Green triangle marking the ball
- **Ball possession**: Red triangle above player with ball
- **Team statistics**: Ball control percentages
- **Camera movement**: X/Y movement display
- **Player metrics**: Speed (km/h) and distance (m) for each player

## Tips and Tricks

### Performance Optimization

1. **Use Cache**: Enable "Use cached data" for faster re-runs
   - First run: Full analysis (slower)
   - Subsequent runs: Uses cached tracking data (much faster)

2. **Video Resolution**: Lower resolution videos process faster
   - Consider downscaling large videos before analysis

3. **Frame Rate**: Higher frame rate videos take longer to process
   - Standard 24-30 fps works well

### Troubleshooting

**Problem: Application won't start**
- Solution: Ensure PyQt5 is installed: `pip install PyQt5`

**Problem: "Model file not found" error**
- Solution: Download and place `best.pt` in the `models/` directory

**Problem: Analysis fails with import errors**
- Solution: Install all dependencies: `pip install -r requirements.txt`

**Problem: Video won't load in player**
- Solution: Ensure video format is supported (MP4, AVI, MOV)
- Try converting to a standard format

**Problem: Out of memory during analysis**
- Solution: 
  - Process shorter video clips
  - Close other applications
  - Use a machine with more RAM

**Problem: Slow performance**
- Solution:
  - Enable caching for re-analysis
  - Use GPU-accelerated YOLO if available
  - Process smaller videos or subsample frames

### Advanced Usage

#### Custom Model

To use a custom trained YOLO model:
1. Select "Custom..." from the Model dropdown
2. Browse to your model file (.pt format)
3. The custom model will be used for detection

#### Batch Processing

To analyze multiple videos:
1. Analyze first video
2. Wait for completion
3. Select next video and repeat
4. Each video generates a separate output file

#### Export Settings

The application saves videos in AVI format with XVID codec by default. To change format, modify the `save_video()` function in `utils/video_utils.py`.

## Keyboard Shortcuts

While the video player is focused:
- **Space**: Play/Pause (when implemented)
- **Left/Right Arrow**: Previous/Next frame (via slider)

## Architecture

The Qt GUI application is built on top of the existing analysis modules:

```
Qt GUI (qt_main.py)
    ↓
Main Window (UI Layout)
    ├── Control Panel
    │   ├── File Selection
    │   ├── Settings
    │   └── Analysis Controls
    ├── Video Player
    │   ├── Display Widget
    │   └── Playback Controls
    └── Analysis Thread (Background)
        └── Analysis Pipeline (existing modules)
```

### Key Components

1. **FootballAnalysisGUI**: Main window class
2. **VideoPlayer**: Widget for video display and playback
3. **VideoAnalysisThread**: Background thread for processing
4. **Qt Signals**: Communication between thread and UI

## Benefits of Qt GUI

### vs Command Line

**GUI Advantages:**
- No code editing required
- Visual feedback during processing
- Built-in video player
- Easy configuration
- Beginner-friendly
- Can stop/cancel processing
- Real-time progress updates

**Command Line Advantages:**
- Scriptable/automatable
- Lower resource usage
- Better for batch processing
- Can run on servers without display

### When to Use GUI

- First-time users
- Interactive analysis sessions
- Testing different settings
- Demonstrating the system
- When you want to preview results immediately

### When to Use Command Line

- Automated workflows
- Batch processing multiple videos
- Running on remote servers
- Scripting and automation
- CI/CD pipelines

## Future Enhancements

Potential improvements for the Qt GUI:
- Real-time video processing preview
- Export statistics to CSV
- Multiple video comparison
- Heat map visualization
- Frame-by-frame annotation editing
- Video trimming tools
- Custom team color selection
- Player statistics dashboard

## Support

For issues or questions:
1. Check this guide for troubleshooting
2. Review the main README.md for technical details
3. Ensure all dependencies are correctly installed
4. Verify video files are in supported formats

## Technical Notes

- **Thread Safety**: Analysis runs in a separate QThread to keep UI responsive
- **Memory Management**: Large videos are processed in chunks
- **Caching**: Pickle files store intermediate results for faster re-runs
- **Video Codec**: Output uses XVID codec for broad compatibility

## Example Workflow

Complete example from start to finish:

```bash
# 1. Install dependencies
pip install -r requirements.txt

# 2. Download and place model file
# (Download best.pt to models/ directory)

# 3. Launch GUI
python qt_main.py

# 4. In the GUI:
#    - Click "Browse..." and select input video
#    - Review settings (defaults are fine)
#    - Click "Start Analysis"
#    - Wait for completion (monitor progress)
#    - Review results in the player
#    - Output saved to output_videos/

# 5. For next video:
#    - Select new input video
#    - Enable "Use cached data" if same video
#    - Start analysis again
```

That's it! You're ready to analyze football matches with the Qt GUI application.
