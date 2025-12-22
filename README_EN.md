# Football Analysis Project

**[中文文檔 / Chinese Documentation](README.md)** | **[English Usage Guide](README_EN.md)** | **[Qt GUI Guide](QT_GUI_GUIDE.md)**

## Introduction
The goal of this project is to detect and track players, referees, and footballs in a video using YOLO, one of the best AI object detection models available. We will also train the model to improve its performance. Additionally, we will assign players to teams based on the colors of their t-shirts using Kmeans for pixel segmentation and clustering. With this information, we can measure a team's ball acquisition percentage in a match. We will also use optical flow to measure camera movement between frames, enabling us to accurately measure a player's movement. Furthermore, we will implement perspective transformation to represent the scene's depth and perspective, allowing us to measure a player's movement in meters rather than pixels. Finally, we will calculate a player's speed and the distance covered. This project covers various concepts and addresses real-world problems, making it suitable for both beginners and experienced machine learning engineers.

**NEW: Now includes a Qt-based GUI application for easy video analysis!**

![Screenshot](output_videos/screenshot.png)

## Table of Contents
- [Qt GUI Application](#qt-gui-application)
- [How It Works](#how-it-works)
- [System Architecture](#system-architecture)
- [Functionality Overview](#functionality-overview)
- [Modules Used](#modules-used)
- [Processing Pipeline](#processing-pipeline)
- [Component Details](#component-details)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)

## Qt GUI Application

The project now includes a comprehensive Qt-based graphical user interface (`qt_main.py`) that makes video analysis easy and interactive.

### GUI Features:
- **Interactive File Selection**: Browse and select input videos with a file picker
- **Real-time Progress Tracking**: Visual progress bar and status updates during analysis
- **Video Player**: Built-in video player to preview input and playback analyzed output
- **Configurable Settings**: 
  - Toggle cache usage for faster re-analysis
  - Select different YOLO models
  - Choose custom output paths
- **Analysis Log**: Real-time log display showing analysis steps and progress
- **Playback Controls**: Play/pause and frame-by-frame navigation of results
- **Modern UI**: Clean, intuitive interface built with PyQt5

### Launching the GUI:
```bash
python qt_main.py
```

### GUI Workflow:
1. Click "Browse..." to select an input video
2. (Optional) Configure settings like cache usage and model selection
3. Click "Start Analysis" to begin processing
4. Monitor progress in real-time through the progress bar and log
5. Once complete, the analyzed video loads automatically in the player
6. Use playback controls to review the analysis results

The GUI runs the analysis in a background thread, keeping the interface responsive during processing.

📖 **[Read the complete Qt GUI Guide](QT_GUI_GUIDE.md)** for detailed instructions, troubleshooting, and tips.

## How It Works

This football analysis system processes video footage of football matches to extract meaningful insights about player movement, team performance, and game dynamics. The system follows a multi-stage pipeline:

### 1. Video Input & Object Detection
- The system reads video frames from the input video file
- Uses a trained YOLO v5 model to detect players, referees, and the ball in each frame
- Processes frames in batches for efficiency

### 2. Object Tracking
- Implements ByteTrack algorithm to maintain consistent tracking IDs across frames
- Tracks players and referees throughout the video
- Handles occlusions and temporary disappearances
- Interpolates ball positions to handle detection gaps

### 3. Team Assignment
- Analyzes the top half of each player's bounding box (jersey area)
- Uses K-means clustering on pixel colors to identify team jerseys
- Assigns players to Team 1 or Team 2 based on jersey color
- Maintains consistent team assignments throughout the match

### 4. Camera Movement Compensation
- Uses optical flow (Lucas-Kanade method) to detect camera movement
- Tracks feature points between consecutive frames
- Adjusts player positions to account for camera panning and movement
- Ensures accurate measurement of actual player movement

### 5. Perspective Transformation
- Converts 2D pixel coordinates to real-world measurements
- Maps court positions to meters using perspective transformation
- Enables accurate distance and speed calculations
- Accounts for camera angle and field perspective

### 6. Ball Possession Analysis
- Calculates distance between each player and the ball
- Assigns ball possession to the nearest player within threshold
- Tracks ball possession over time
- Calculates ball control percentage for each team

### 7. Speed & Distance Calculation
- Measures distance traveled by each player in meters
- Calculates player speed in km/h
- Updates measurements in real-time throughout the video
- Displays speed and distance for each tracked player

### 8. Visualization & Output
- Draws ellipses around players with team colors
- Shows player IDs and tracking information
- Displays ball possession indicators
- Shows camera movement, speed, distance, and team statistics
- Generates annotated output video

## System Architecture

```
Input Video
    |
    v
[Video Reader] --> Frames
    |
    v
[YOLO Detection] --> Raw Detections
    |
    v
[ByteTrack] --> Tracked Objects (Players, Referees, Ball)
    |
    +---> [Team Assigner] --> Team Classifications
    |
    +---> [Camera Movement Estimator] --> Camera Adjustments
    |
    +---> [View Transformer] --> Real-world Coordinates
    |
    +---> [Ball Assigner] --> Ball Possession Data
    |
    +---> [Speed & Distance Estimator] --> Performance Metrics
    |
    v
[Visualization Engine] --> Annotated Frames
    |
    v
[Video Writer] --> Output Video
```

## Functionality Overview

### Core Features

1. **Multi-Object Detection & Tracking**
   - Detects and tracks multiple players, referees, and ball simultaneously
   - Maintains unique IDs for each tracked object
   - Handles partial occlusions and re-identification

2. **Team Identification**
   - Automatically identifies two teams based on jersey colors
   - Uses unsupervised learning (K-means) for color clustering
   - Visual indication of team assignment with colored ellipses

3. **Ball Possession Tracking**
   - Real-time ball possession assignment
   - Distance-based possession calculation
   - Team ball control statistics and percentages

4. **Movement Analysis**
   - Camera movement detection and compensation
   - Real-world distance calculation (in meters)
   - Player speed calculation (in km/h)
   - Cumulative distance tracking for each player

5. **Visual Analytics**
   - Player tracking with colored ellipses
   - Team identification colors
   - Ball possession indicators (triangles)
   - On-screen statistics display
   - Camera movement indicators

## Modules Used
The following modules are used in this project:
- **YOLO v5**: State-of-the-art object detection model for identifying players, referees, and ball
- **ByteTrack**: Multi-object tracking algorithm for maintaining consistent IDs
- **K-means Clustering**: Unsupervised learning for team color identification
- **Optical Flow (Lucas-Kanade)**: Camera movement detection between frames
- **Perspective Transformation**: Converting pixel coordinates to real-world measurements
- **OpenCV**: Computer vision operations and video processing

## Processing Pipeline

### Step-by-Step Process:

1. **Video Loading** (`utils/video_utils.py`)
   - Read video file frame by frame
   - Store frames in memory for processing

2. **Object Detection** (`trackers/tracker.py`)
   - Initialize YOLO model with trained weights
   - Detect objects in batches (batch_size=20)
   - Convert detections to supervision format

3. **Object Tracking** (`trackers/tracker.py`)
   - Apply ByteTrack algorithm
   - Assign and maintain tracking IDs
   - Interpolate missing ball positions

4. **Position Calculation** (`utils/bbox_utils.py`)
   - Calculate center points for ball
   - Calculate foot positions for players
   - Store position data in tracks

5. **Camera Movement Analysis** (`camera_movement_estimator/`)
   - Extract feature points from first frame
   - Track features using Lucas-Kanade optical flow
   - Calculate camera movement vectors
   - Adjust all object positions

6. **Perspective Transformation** (`view_transformer/`)
   - Define court pixel vertices and real-world dimensions
   - Apply perspective transformation matrix
   - Convert adjusted positions to meters

7. **Team Assignment** (`team_assigner/`)
   - Extract jersey colors from first frame
   - Cluster colors into two teams using K-means
   - Assign team IDs to all players
   - Maintain team assignments throughout video

8. **Ball Possession** (`player_ball_assigner/`)
   - Calculate distance from each player to ball
   - Assign ball to nearest player within threshold (70 pixels)
   - Track possession changes over time

9. **Speed & Distance Calculation** (`speed_and_distance_estimator/`)
   - Measure distance between frame windows (5 frames)
   - Calculate speed based on distance and time
   - Accumulate total distance traveled
   - Convert to km/h and meters

10. **Visualization** (`trackers/tracker.py`)
    - Draw ellipses around players (color-coded by team)
    - Draw triangles for ball and possession indicator
    - Display team statistics and ball control
    - Show camera movement data
    - Show speed and distance for each player

11. **Output Generation** (`utils/video_utils.py`)
    - Write annotated frames to output video
    - Save as AVI format with XVID codec

## Component Details

### 1. Tracker (`trackers/tracker.py`)
**Purpose**: Detect and track objects throughout the video

**Key Methods**:
- `detect_frames()`: Batch detection using YOLO
- `get_object_tracks()`: Generate tracking data with ByteTrack
- `interpolate_ball_positions()`: Fill gaps in ball detection
- `draw_ellipse()`: Visual representation of players
- `draw_triangle()`: Visual representation of ball
- `draw_team_ball_control()`: Display team statistics
- `draw_annotations()`: Complete visualization pipeline

**Features**:
- Goalkeeper detection and conversion to player class
- Stub file caching for faster re-runs
- Batch processing for efficiency

### 2. Team Assigner (`team_assigner/team_assigner.py`)
**Purpose**: Identify and assign players to teams based on jersey colors

**Key Methods**:
- `get_player_color()`: Extract dominant jersey color
- `get_clustering_model()`: Create K-means model
- `assign_team_color()`: Initial team color assignment
- `get_player_team()`: Assign individual player to team

**Algorithm**:
1. Crop top half of player bounding box (jersey area)
2. Apply K-means clustering to identify jersey vs background
3. Extract player cluster color
4. Cluster all player colors into 2 teams
5. Assign team IDs consistently

### 3. Player Ball Assigner (`player_ball_assigner/player_ball_assigner.py`)
**Purpose**: Determine which player has possession of the ball

**Key Methods**:
- `assign_ball_to_player()`: Calculate nearest player to ball

**Algorithm**:
1. Get ball center position
2. Calculate distance from ball to each player's feet
3. Check both left and right foot distances
4. Assign to nearest player if within 70 pixels
5. Return player ID or -1 if no possession

### 4. Camera Movement Estimator (`camera_movement_estimator/camera_movement_estimator.py`)
**Purpose**: Detect and compensate for camera movement

**Key Methods**:
- `get_camera_movement()`: Calculate movement vectors per frame
- `add_adjust_positions_to_tracks()`: Apply corrections to positions
- `draw_camera_movement()`: Visualize camera movement

**Algorithm**:
1. Extract good features to track from first frame
2. Use Lucas-Kanade optical flow to track features
3. Calculate maximum feature displacement
4. Store X,Y movement vectors
5. Adjust all object positions by camera movement

### 5. View Transformer (`view_transformer/view_transformer.py`)
**Purpose**: Convert pixel coordinates to real-world measurements

**Key Methods**:
- `transform_point()`: Convert single point using perspective matrix
- `add_transformed_position_to_tracks()`: Transform all positions

**Configuration**:
- Court width: 68 meters
- Court length: 23.32 meters
- Four reference points map pixels to real-world coordinates

### 6. Speed and Distance Estimator (`speed_and_distance_estimator/speed_and_distance_estimator.py`)
**Purpose**: Calculate player speed and distance traveled

**Key Methods**:
- `add_speed_and_distance_to_tracks()`: Calculate metrics
- `draw_speed_and_distance()`: Display on video

**Configuration**:
- Frame window: 5 frames
- Frame rate: 24 fps
- Speed in km/h
- Distance in meters

**Algorithm**:
1. Measure distance between positions at frame window interval
2. Calculate time elapsed based on frame rate
3. Calculate speed (distance/time)
4. Accumulate total distance
5. Update all frames in window with current metrics

### 7. Utility Functions (`utils/`)
**bbox_utils.py**:
- `get_center_of_bbox()`: Calculate bounding box center
- `get_bbox_width()`: Calculate bounding box width
- `measure_distance()`: Euclidean distance between points
- `measure_xy_distance()`: X,Y distance components
- `get_foot_position()`: Calculate bottom-center of bbox

**video_utils.py**:
- `read_video()`: Load video into frame list
- `save_video()`: Write frames to video file

## Trained Models
- [Trained Yolo v5](https://drive.google.com/file/d/1DC2kCygbBWUKheQ_9cFziCsYVSRw6axK/view?usp=sharing)

## Sample video
-  [Sample input video](https://drive.google.com/file/d/1t6agoqggZKx6thamUuPAIdN_1zR9v9S_/view?usp=sharing)

## Requirements
To run this project, you need to have the following requirements installed:
- Python 3.x
- ultralytics (YOLO implementation)
- supervision (tracking utilities)
- OpenCV (cv2) - Computer vision operations
- NumPy - Numerical computations
- Matplotlib - Visualization
- Pandas - Data manipulation
- scikit-learn (sklearn) - K-means clustering
- PyQt5 (GUI framework) - **Required for Qt GUI application**

## Installation

```bash
# Clone the repository
git clone https://github.com/vesss1/foot.git
cd foot

# Install required packages
pip install -r requirements.txt

# Or install manually:
pip install ultralytics supervision opencv-python numpy matplotlib pandas scikit-learn PyQt5

# Download the trained model
# Place the model file in the models/ directory as models/best.pt

# Download sample video (optional)
# Place input video in input_videos/ directory
```

### Opening in Qt Creator (IDE)

If you want to use Qt Creator IDE to view and edit the project:

1. Open Qt Creator
2. Click "File" → "Open File or Project..."
3. Navigate to the project directory and select `foot.pro`
4. Qt Creator will load the project structure with all Python files organized

**Note**: This is a Python project using PyQt5. Qt Creator provides an IDE environment for viewing and editing the code, but you'll still run the Python scripts using the standard Python interpreter as described in the Usage section below.

📖 **[Read the complete Qt Creator Integration Guide](QT_CREATOR_SETUP.md)** for detailed installation and configuration instructions.

## Usage

### Option 1: Qt GUI Application (Recommended)

```bash
# Launch the graphical interface
python qt_main.py
```

**GUI Usage:**
1. Click "Browse..." to select your input video file
2. (Optional) Adjust settings:
   - Enable/disable cache for faster re-runs
   - Select a different YOLO model
   - Change output file location
3. Click "Start Analysis" to begin processing
4. Monitor real-time progress through the progress bar and log
5. Once complete, use the built-in player to review results
6. Use playback controls to play/pause and navigate frames

**Benefits of GUI:**
- No code editing required
- Visual feedback during processing
- Built-in video player for results
- Easy configuration management
- Beginner-friendly interface

### Option 2: Command Line Script

```bash
# Run the analysis script
python main.py
```

The script will:
1. Read the input video from `input_videos/08fd33_4.mp4`
2. Process all frames through the pipeline
3. Generate annotated output video at `output_videos/output_video.avi`

**Note**: First run may take longer. Subsequent runs use cached stub files for faster processing.

### Configuration Options in main.py:
- `read_from_stub=True`: Use cached tracking/camera data
- `stub_path`: Path to cache files
- Video input path
- Output video path

## Output

The generated video includes:
- Color-coded player tracking (ellipses with team colors)
- Player IDs on jerseys
- Ball tracking (green triangle)
- Ball possession indicator (red triangle above player)
- Team ball control percentages
- Camera movement X/Y coordinates
- Individual player speed (km/h)
- Individual player distance traveled (meters)

## Project Structure

```
foot/
├── main.py                          # Command-line execution script
├── qt_main.py                       # Qt GUI application (NEW)
├── requirements.txt                 # Python dependencies (NEW)
├── yolo_inference.py                # YOLO inference utilities
├── models/                          # Trained model files
│   └── best.pt                      # YOLO v5 trained weights
├── input_videos/                    # Input video files
├── output_videos/                   # Generated output videos
├── stubs/                           # Cached processing data
│   ├── track_stubs.pkl             # Cached tracking data
│   └── camera_movement_stub.pkl    # Cached camera movement
├── trackers/                        # Object detection and tracking
│   ├── __init__.py
│   └── tracker.py
├── team_assigner/                   # Team identification
│   ├── __init__.py
│   └── team_assigner.py
├── player_ball_assigner/            # Ball possession logic
│   ├── __init__.py
│   └── player_ball_assigner.py
├── camera_movement_estimator/       # Camera movement detection
│   ├── __init__.py
│   └── camera_movement_estimator.py
├── view_transformer/                # Perspective transformation
│   ├── __init__.py
│   └── view_transformer.py
├── speed_and_distance_estimator/    # Performance metrics
│   ├── __init__.py
│   └── speed_and_distance_estimator.py
└── utils/                           # Helper functions
    ├── __init__.py
    ├── bbox_utils.py               # Bounding box utilities
    └── video_utils.py              # Video I/O utilities
```

## Technical Details

### Performance Optimizations:
- Batch processing of frames for YOLO detection
- Stub file caching for tracking and camera movement data
- Efficient numpy operations for transformations
- Frame window approach for speed/distance calculations

### Accuracy Considerations:
- Ball position interpolation handles detection gaps
- Camera movement compensation ensures accurate measurements
- Perspective transformation provides real-world coordinates
- Multiple distance checks for ball possession assignment

### Limitations:
- Requires calibration points for perspective transformation.
- Performance depends on video quality and camera angle.
- Jersey color detection may fail with similar team colors.
- Assumes relatively stable camera movement.

## Future Enhancements

Potential improvements:
- Real-time processing capabilities
- Support for multiple camera angles
- Advanced player statistics (heat maps, pass detection)
- Automatic camera calibration
- Deep learning-based team assignment
- Player identification and jersey number recognition
- Tactical analysis and formation detection