# Football Analysis Qt GUI - User Interface Design

## Main Window Layout

```
┌─────────────────────────────────────────────────────────────────────┐
│  Football Analysis - Qt GUI                                    [_][□][X] │
├─────────────────────────────────────────────────────────────────────┤
│                                                                       │
│  ┌─ Input Configuration ───────────────────────────────────────┐    │
│  │                                                               │    │
│  │  Input Video:      [/path/to/video.mp4        ] [Browse...]  │    │
│  │                                                               │    │
│  │  Model File:       [/path/to/best.pt          ] [Browse...]  │    │
│  │                                                               │    │
│  │  Output Directory: [/path/to/output_videos    ] [Browse...]  │    │
│  │                                                               │    │
│  │  ☑ Use Cached Stubs (faster processing)                      │    │
│  │                                                               │    │
│  └───────────────────────────────────────────────────────────────┘    │
│                                                                       │
│  ┌─ Process Output ─────────────────────────────────────────────┐    │
│  │ =================================================            │    │
│  │ Starting Football Analysis...                                │    │
│  │ Time: 2026-01-01 15:18:00                                   │    │
│  │ =================================================            │    │
│  │ Input Video: /path/to/video.mp4                             │    │
│  │ Model File: /path/to/best.pt                                │    │
│  │ Output Directory: /path/to/output_videos                     │    │
│  │ Use Stubs: Yes                                               │    │
│  │                                                              │    │
│  │ Command: python3 /tmp/foot_analysis_wrapper.py              │    │
│  │ Working Directory: /path/to/foot-Function                    │    │
│  │ =================================================            │    │
│  │                                                              │    │
│  │ 2026-01-01 15:18:01 - INFO - Starting pipeline...          │    │
│  │ 2026-01-01 15:18:02 - INFO - Reading video...              │    │
│  │ 2026-01-01 15:18:05 - INFO - Successfully read 300 frames  │    │
│  │ 2026-01-01 15:18:06 - INFO - Initializing tracker...       │    │
│  │ ...                                                          │    │
│  │ (scrollable output area)                                     │    │
│  └──────────────────────────────────────────────────────────────┘    │
│                                                                       │
│                  [ Run Analysis ]    [ Stop ]                         │
│                                                                       │
├─────────────────────────────────────────────────────────────────────┤
│  Ready                                                                │
└─────────────────────────────────────────────────────────────────────┘
```

## UI Components Description

### Input Configuration Group
- **Input Video Field**: Text input with Browse button for video file selection
- **Model File Field**: Text input with Browse button for YOLO model selection
- **Output Directory Field**: Text input with Browse button for output folder
- **Use Stubs Checkbox**: Toggle for using cached stub files

### Process Output Group
- **Output Console**: Read-only text area with:
  - Monospace font for better readability
  - Auto-scroll to bottom
  - Color-coded output:
    - Black: Normal stdout messages
    - Red: Error messages (stderr)
  - Line wrapping disabled for log formatting

### Control Buttons
- **Run Analysis**: Large button to start the analysis
  - Disabled when process is running
  - Bold styling to emphasize primary action
- **Stop**: Button to terminate running process
  - Enabled only when process is running

### Status Bar
- Shows current application state:
  - "Ready" - Idle state
  - "Starting process..." - Initializing
  - "Running analysis..." - Processing
  - "Analysis completed successfully" - Success
  - "Error: [message]" - Error state

## User Interaction Flow

### 1. Launch Application
```
User launches FootAnalysisGUI
   ↓
Application initializes
   ↓
Main window appears with default values
   ↓
Status: "Ready"
```

### 2. Configure Input Parameters
```
User clicks "Browse..." for input video
   ↓
File dialog opens (filtered for video files)
   ↓
User selects video file
   ↓
Path populated in input field

(Repeat for model file and output directory)
```

### 3. Run Analysis
```
User clicks "Run Analysis"
   ↓
Application validates inputs
   - File existence checks
   - Directory access checks
   - foot-Function availability
   ↓
If validation fails:
   → Show error dialog
   → Remain in Ready state
   ↓
If validation passes:
   → Create Python wrapper script
   → Set working directory
   → Start QProcess with python3
   ↓
Process starts
   ↓
Status: "Running analysis..."
Controls disabled (except Stop button)
   ↓
Real-time output displayed:
   - stdout in black
   - stderr in red
   - Auto-scroll enabled
   ↓
Process completes
   ↓
If successful (exit code 0):
   → Show success dialog
   → Status: "Analysis completed successfully"
   ↓
If failed (non-zero exit code):
   → Status: "Process failed with exit code X"
   ↓
Controls re-enabled
```

### 4. Stop Running Process (Optional)
```
User clicks "Stop" while process is running
   ↓
Application sends terminate signal
   ↓
Wait 5 seconds for graceful shutdown
   ↓
If not terminated:
   → Force kill process
   ↓
Status: "Process crashed or terminated"
Controls re-enabled
```

## Error Handling

### Validation Errors (before process start)
- **Empty input video**: "Please specify an input video file."
- **Video not found**: "Input video file does not exist: [path]"
- **Empty model file**: "Please specify a model file."
- **Model not found**: "Model file does not exist: [path]"
- **Empty output dir**: "Please specify an output directory."
- **Cannot create output dir**: "Failed to create output directory: [path]"
- **foot-Function not found**: "foot-Function directory not found: [path]"
- **main.py not found**: "main.py not found in foot-Function directory"

### Process Errors (during execution)
- **Failed to start**: "Failed to start process. Check if Python 3 is installed."
- **Process crashed**: "Process crashed."
- **Timeout**: "Process timed out."

### Success Messages
- **Analysis complete**: 
  ```
  Football analysis completed successfully!
  
  Output files saved to:
  /path/to/output_videos
  ```

## Example Session Output

```
==========================================================
Starting Football Analysis...
Time: 2026-01-01 15:18:00
==========================================================
Input Video: /home/user/foot/foot-Function/input_videos/08fd33_4.mp4
Model File: /home/user/foot/foot-Function/models/best.pt
Output Directory: /home/user/foot/foot-Function/output_videos
Use Stubs: Yes

Command: python3 /tmp/foot_analysis_wrapper.py
Working Directory: /home/user/foot/foot-Function
==========================================================

2026-01-01 15:18:01,123 - __main__ - INFO - ============================================================
2026-01-01 15:18:01,124 - __main__ - INFO - Starting Football Analysis Pipeline
2026-01-01 15:18:01,125 - __main__ - INFO - ============================================================
2026-01-01 15:18:01,126 - __main__ - INFO - Input validation passed
2026-01-01 15:18:01,127 - __main__ - INFO - Output directory ready: output_videos
2026-01-01 15:18:01,128 - __main__ - INFO - Reading video: input_videos/08fd33_4.mp4
2026-01-01 15:18:05,234 - __main__ - INFO - Successfully read 300 frames
2026-01-01 15:18:05,235 - __main__ - INFO - Initializing tracker
2026-01-01 15:18:06,456 - __main__ - INFO - Getting object tracks
2026-01-01 15:18:10,789 - __main__ - INFO - Object tracking complete
2026-01-01 15:18:10,790 - __main__ - INFO - Processing camera movement
2026-01-01 15:18:15,123 - __main__ - INFO - Camera movement processing complete
2026-01-01 15:18:15,124 - __main__ - INFO - Applying view transformation
2026-01-01 15:18:15,567 - __main__ - INFO - View transformation complete
2026-01-01 15:18:15,568 - __main__ - INFO - Interpolating ball positions
2026-01-01 15:18:15,890 - __main__ - INFO - Ball position interpolation complete
2026-01-01 15:18:15,891 - __main__ - INFO - Calculating speed and distance
2026-01-01 15:18:16,234 - __main__ - INFO - Speed and distance calculation complete
2026-01-01 15:18:16,235 - __main__ - INFO - Assigning player teams
2026-01-01 15:18:18,567 - __main__ - INFO - Team assignment complete
2026-01-01 15:18:18,568 - __main__ - INFO - Assigning ball possession
2026-01-01 15:18:19,123 - __main__ - INFO - Ball possession assignment complete
2026-01-01 15:18:19,124 - __main__ - INFO - Drawing annotations
2026-01-01 15:18:25,678 - __main__ - INFO - Annotation drawing complete
2026-01-01 15:18:25,679 - __main__ - INFO - Saving output video to: output_videos/output_video.avi
2026-01-01 15:18:35,123 - __main__ - INFO - Output video saved successfully (145.67 MB)
2026-01-01 15:18:35,124 - __main__ - INFO - Saving output data to: output_videos/data_output.json
2026-01-01 15:18:35,456 - __main__ - INFO - Output data saved successfully
2026-01-01 15:18:35,457 - __main__ - INFO - ============================================================
2026-01-01 15:18:35,458 - __main__ - INFO - Pipeline completed successfully!
2026-01-01 15:18:35,459 - __main__ - INFO - Video output: output_videos/output_video.avi
2026-01-01 15:18:35,460 - __main__ - INFO - Data output: output_videos/data_output.json
2026-01-01 15:18:35,461 - __main__ - INFO - ============================================================

=== Analysis completed successfully ===

==========================================================
Process completed successfully!
==========================================================
```

## Key Features Demonstrated

1. **No Modification to foot-Function**: All integration done via QProcess
2. **Real-time Output Capture**: stdout/stderr displayed as they arrive
3. **Error Handling**: Comprehensive validation and error reporting
4. **User-Friendly**: Clear status messages and dialogs
5. **Process Control**: Ability to start/stop analysis
6. **File Dialogs**: Easy browsing for input/output files
7. **Configuration Persistence**: Default values pre-populated
