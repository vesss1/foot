# Quick Start Guide - Football Analysis GUI

## 🚀 Get Started in 5 Minutes

### Prerequisites Check
Before you begin, ensure you have:
- [ ] Qt Creator 18.0.0 or later installed
- [ ] Qt 6.10.x (or compatible Qt6) with Multimedia modules
- [ ] Python 3.8+ in your system PATH
- [ ] Python packages: `opencv-python`, `numpy`, `torch`, `ultralytics`

### Step 1: Build the Application (2 minutes)

#### Option A: Qt Creator (Recommended)
1. Launch Qt Creator
2. Open `FootAnalysisGUI.pro`
3. Select Qt 6.10.x Desktop kit
4. Click **Build** (hammer icon or Ctrl+B)
5. Click **Run** (green play icon or Ctrl+R)

#### Option B: Command Line (Windows)
```cmd
cd path\to\foot
call "C:\Qt\6.10.0\msvc2019_64\bin\qtenv2.bat"
qmake FootAnalysisGUI.pro
nmake
release\FootAnalysisGUI.exe
```

#### Option C: Command Line (Linux)
```bash
cd /path/to/foot
qmake6 FootAnalysisGUI.pro
make
./FootAnalysisGUI
```

### Step 2: Prepare Your Data (1 minute)

You need:
1. **Input Video**: A football/soccer match video file (MP4, AVI, MOV, MKV)
2. **YOLO Model**: A trained YOLO model file (e.g., `best.pt`)

💡 **Tip**: Sample data should be in `foot-Function/input_videos/` and `foot-Function/models/`

### Step 3: Run Analysis (1 minute setup)

1. **Launch the application** (if not already running)

2. **Select Input Video**:
   - Click **Browse...** next to "Input Video"
   - Navigate to your video file
   - Select and click **Open**

3. **Select YOLO Model**:
   - Click **Browse...** next to "YOLO Model"
   - Navigate to your model file (e.g., `best.pt`)
   - Select and click **Open**

4. **Start Analysis**:
   - Click the **Start Analysis** button
   - Analysis begins immediately

### Step 4: Monitor Progress (During Analysis)

Watch the **Analysis Log** section for real-time output:
```
=== Analysis Started ===
Command: python main.py --input video.mp4 --model best.pt
Reading video: video.mp4
Successfully read 450 frames
Initializing tracker...
Getting object tracks...
Processing camera movement...
[... more progress messages ...]
```

**What's happening?**
- Python script is running in the background
- Video is being analyzed frame-by-frame
- Players are being tracked and classified
- Speed and distance are being calculated

**UI stays responsive!** You can:
- Resize the window
- Scroll through the log
- View other applications
- Wait patiently ☕

### Step 5: View Results (1 minute)

When analysis completes, results load automatically:

#### 📊 Data Table Tab
- **Automatically switches to this tab**
- Shows player statistics:
  - Team assignment (team_1, team_2)
  - Player IDs
  - Distance covered (meters)
- Shows team possession percentages
- Sortable columns, searchable data

#### 🎥 Video Output Tab
- Click the **Video Output** tab
- Video is already loaded
- Click **Play** to watch annotated video
- Features:
  - Player bounding boxes with team colors
  - Ball tracking
  - Speed and distance overlays
  - Possession indicators
- Controls:
  - **Play/Pause**: Toggle playback
  - **Stop**: Reset to beginning

#### 📝 Summary Tab
- Shows overall status message
- Confirms analysis completion
- Provides file paths if needed

## 🎯 What You Should See

### During Analysis (30-120 seconds typical)
```
╔══════════════════════════════════════════════════════════════════╗
║  Status: Running analysis...                                     ║
╠══════════════════════════════════════════════════════════════════╣
║  Analysis Log:                                                   ║
║  === Analysis Started ===                                        ║
║  Reading video: input.mp4                                        ║
║  Successfully read 450 frames                                    ║
║  Initializing tracker...                                         ║
║  Object tracking complete                                        ║
║  Processing camera movement...                                   ║
║  [... more output ...]                                           ║
╚══════════════════════════════════════════════════════════════════╝
```

### After Analysis Completes
```
╔══════════════════════════════════════════════════════════════════╗
║  Status: Analysis completed successfully                         ║
╠══════════════════════════════════════════════════════════════════╣
║  Results Tabs:                                                   ║
║  [Summary] [Data Table ✓] [Video Output]                        ║
║                                                                  ║
║  Player Statistics and Team Possession                           ║
║  ┌────────────┬─────────────┬──────────────────────────────┐   ║
║  │ Team       │ Player ID   │ Distance (m)                 │   ║
║  ├────────────┼─────────────┼──────────────────────────────┤   ║
║  │ team_1     │ 2           │ 1234.56                      │   ║
║  │ team_1     │ 5           │ 987.65                       │   ║
║  │ team_2     │ 3           │ 1089.12                      │   ║
║  │ ...        │ ...         │ ...                          │   ║
║  │ Team 1 Possession        │ 58.23%                       │   ║
║  │ Team 2 Possession        │ 41.77%                       │   ║
║  └────────────┴─────────────┴──────────────────────────────┘   ║
╚══════════════════════════════════════════════════════════════════╝
```

## 🔧 Troubleshooting

### Build Issues

**Error: "Qt Multimedia module not found"**
```
Solution: Install Qt Multimedia
- Open Qt Maintenance Tool
- Modify Qt 6.10.x installation
- Check: Qt Multimedia and Qt MultimediaWidgets
- Apply changes and rebuild
```

**Error: "Python not found"**
```
Solution: Add Python to PATH
- Windows: System Properties → Environment Variables → Add Python to PATH
- Linux/Mac: Usually already in PATH
- Test: Open terminal and run `python --version`
```

### Runtime Issues

**Video shows black screen**
```
Solution: Install video codecs
- Windows: Download and install K-Lite Codec Pack
- Linux: sudo apt-get install gstreamer1.0-plugins-good gstreamer1.0-plugins-bad
- Restart application
```

**"Output files not found"**
```
Solution: Check Python analysis completed
1. Look in Analysis Log for errors
2. Verify Python script finished (exit code 0)
3. Check foot-Function/output_videos/ directory exists
4. Try running Python script manually to debug
```

**Analysis fails immediately**
```
Solution: Verify inputs
1. Check input video file is valid (try playing it in VLC)
2. Check model file exists and is correct format (.pt)
3. Check Python dependencies installed: pip install opencv-python numpy torch ultralytics
4. Read error message in Analysis Log
```

## 💡 Tips & Tricks

### Speed Up Analysis
- **Use stubs**: The Python script caches results in `stubs/` directory
- First run is slow, subsequent runs with same video are much faster
- To force fresh analysis: Delete `foot-Function/stubs/*.pkl`

### Working with Results

**Export Data**:
- Data Table: Select all (Ctrl+A), Copy (Ctrl+C), Paste into Excel
- Files are also saved:
  - CSV: `foot-Function/output_videos/data_output.csv`
  - JSON: `foot-Function/output_videos/data_output.json`
  - Video: `foot-Function/output_videos/output_video.avi`

**Share Results**:
- Video file can be opened in any player (VLC, Media Player)
- CSV can be imported into Excel, Google Sheets
- JSON can be processed by other tools

### Advanced Usage

**Custom Python Arguments**:
- Modify `MainWindow.cpp` line ~200 to add arguments
- Example: Add `--no-cache` to disable stubs

**Different Output Location**:
- Modify `MainWindow.cpp` line ~357 to change output path
- Or create symbolic link to desired location

## 📚 Need More Help?

- **Detailed Build Instructions**: See `BUILD_INSTRUCTIONS.md`
- **Full Documentation**: See `README.md`
- **Implementation Details**: See `IMPLEMENTATION_SUMMARY.md`
- **UI Structure**: See `UI_STRUCTURE.md`
- **Change Log**: See `CHANGES.md`

## 🎬 Typical Workflow Example

```
1. Launch GUI                    [5 seconds]
2. Browse → Select video.mp4     [10 seconds]
3. Browse → Select best.pt       [5 seconds]
4. Click "Start Analysis"        [1 second]
   → Wait for analysis          [30-120 seconds]
5. View Data Table               [automatic]
6. Click "Video Output" tab      [1 second]
7. Click "Play"                  [1 second]
8. Watch annotated video         [as long as video]
9. Review player statistics      [as needed]

Total active time: ~25 seconds
Total analysis time: 30-120 seconds (automated)
```

## ✅ Success Checklist

After first successful run, you should have:
- [x] Application built and running
- [x] Analysis completed without errors
- [x] Data Table showing player statistics
- [x] Team possession percentages displayed
- [x] Video Output tab playing annotated video
- [x] Output files saved in `foot-Function/output_videos/`

**Congratulations!** 🎉 You're now analyzing football matches with computer vision!

## 🚀 Next Steps

- Try different videos to see how the analysis adapts
- Compare statistics across multiple matches
- Export data for further analysis in Excel/Python
- Share annotated videos with coaches or analysts
- Read implementation docs to understand the algorithms

---

**Questions?** Check the documentation files or review the Analysis Log for detailed error messages.

**Happy Analyzing!** ⚽🏃‍♂️📊
