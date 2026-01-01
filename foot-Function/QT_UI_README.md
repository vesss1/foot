# Football Analysis Qt UI

This is a Qt-based graphical user interface for viewing football analysis results.

## Features

- **Video Playback**: Watch the analyzed football video with annotations
- **Ball Possession Statistics**: View percentage of ball control for Team 1 and Team 2
- **Running Statistics**: 
  - Total running distance (kilometers)
  - Average distance per player
  - Average speed (km/h)
- **Time Statistics**: Video duration and timing information

## Requirements

- Python 3.8+
- PyQt5
- OpenCV (cv2)
- Other dependencies from the main analysis pipeline

## Installation

Install PyQt5 if not already installed:

```bash
pip install PyQt5
```

## Usage

### Option 1: Launch UI Only (if analysis already done)

```bash
python qt_ui_app.py
```

or

```bash
python run_analysis_with_ui.py
```

### Option 2: Run Analysis Then Launch UI

```bash
python run_analysis_with_ui.py --run-analysis
```

### Option 3: Run Analysis Only (no UI)

```bash
python run_analysis_with_ui.py --run-analysis --no-ui
```

## File Structure

- `qt_ui_app.py`: Main Qt UI application
- `run_analysis_with_ui.py`: Launcher script that can run analysis and/or launch UI
- `output_videos/`: Directory containing analysis output
  - `output_video.avi`: Annotated video output
  - `data_output.json`: Statistical data
  - `data_output.csv`: Statistical data in CSV format

## UI Components

### Video Player Panel
- Video display area
- Playback controls (Play/Pause/Stop)
- Position slider for seeking
- Load video file button

### Statistics Panel
- **Ball Possession**: Team 1 and Team 2 percentages
- **Running Statistics**: Total and average distances
- **Time Statistics**: Video duration and average speed
- Refresh button to reload statistics

## Auto-Loading

The UI automatically loads files from the `output_videos` directory:
- `output_video.avi`: Video file
- `data_output.json`: Statistics file

If these files exist, they will be loaded automatically when the application starts.

## Screenshots

(Screenshots would be added here after running the application)

## Troubleshooting

### Video won't play
- Ensure the video codec is supported by your system
- Try converting the video to a different format (e.g., MP4)

### Statistics not showing
- Verify that `data_output.json` exists in the `output_videos` directory
- Click "Refresh Statistics" button
- Run the analysis pipeline first

### PyQt5 not found
```bash
pip install PyQt5
```
