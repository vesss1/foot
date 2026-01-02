# UI Structure Diagram

## Main Window Layout

```
╔═══════════════════════════════════════════════════════════════════════════╗
║                        Foot Analysis GUI                                  ║
║                                                                           ║
║  ╔═══════════════════════════════════════════════════════════════════╗  ║
║  ║  Input Video                                                       ║  ║
║  ║  ┌──────────────────────────────────────────┐  ┌──────────────┐  ║  ║
║  ║  │ Select input video file...                │  │  Browse...   │  ║  ║
║  ║  └──────────────────────────────────────────┘  └──────────────┘  ║  ║
║  ╚═══════════════════════════════════════════════════════════════════╝  ║
║                                                                           ║
║  ╔═══════════════════════════════════════════════════════════════════╗  ║
║  ║  YOLO Model                                                        ║  ║
║  ║  ┌──────────────────────────────────────────┐  ┌──────────────┐  ║  ║
║  ║  │ Select YOLO model file...                 │  │  Browse...   │  ║  ║
║  ║  └──────────────────────────────────────────┘  └──────────────┘  ║  ║
║  ╚═══════════════════════════════════════════════════════════════════╝  ║
║                                                                           ║
║  ┌─────────────────────────────────────────────────────────────────────┐ ║
║  │                      Start Analysis                                  │ ║
║  └─────────────────────────────────────────────────────────────────────┘ ║
║                                                                           ║
║  ┌─────────────────────────────────────────────────────────────────────┐ ║
║  │  Status: Analysis completed successfully                             │ ║
║  └─────────────────────────────────────────────────────────────────────┘ ║
║                                                                           ║
║  ╔═══════════════════════════════════════════════════════════════════╗  ║
║  ║  Analysis Log                                                      ║  ║
║  ║  ┌───────────────────────────────────────────────────────────────┐ ║  ║
║  ║  │ === Analysis Started ===                                       │ ║  ║
║  ║  │ Command: python main.py --input video.mp4 --model best.pt     │ ║  ║
║  ║  │ Reading video: video.mp4                                       │ ║  ║
║  ║  │ Successfully read 450 frames                                   │ ║  ║
║  ║  │ Initializing tracker...                                        │ ║  ║
║  ║  │ [Real-time stdout/stderr output appears here...]               │ ║  ║
║  ║  └───────────────────────────────────────────────────────────────┘ ║  ║
║  ╚═══════════════════════════════════════════════════════════════════╝  ║
║                                                                           ║
║  ╔═══════════════════════════════════════════════════════════════════╗  ║
║  ║  Analysis Results                                                  ║  ║
║  ║  ┌──────────┬──────────────┬──────────────────────────────────┐  ║  ║
║  ║  │ Summary  │ Data Table   │ Video Output                      │  ║  ║
║  ║  └──────────┴──────────────┴──────────────────────────────────┘  ║  ║
║  ║  [Tab content area - see detailed views below]                    ║  ║
║  ╚═══════════════════════════════════════════════════════════════════╝  ║
╚═══════════════════════════════════════════════════════════════════════════╝
```

## Tab 1: Summary View

```
╔═══════════════════════════════════════════════════════════════════════════╗
║ Summary | Data Table | Video Output                                       ║
╠═══════════════════════════════════════════════════════════════════════════╣
║                                                                           ║
║                                                                           ║
║          ┌───────────────────────────────────────────────────┐           ║
║          │                                                   │           ║
║          │       Analysis complete!                          │           ║
║          │                                                   │           ║
║          │       Check the Data Table and Video Output      │           ║
║          │       tabs to view results.                      │           ║
║          │                                                   │           ║
║          └───────────────────────────────────────────────────┘           ║
║                                                                           ║
║                                                                           ║
╚═══════════════════════════════════════════════════════════════════════════╝
```

## Tab 2: Data Table View

```
╔═══════════════════════════════════════════════════════════════════════════╗
║ Summary | Data Table | Video Output                                       ║
╠═══════════════════════════════════════════════════════════════════════════╣
║                                                                           ║
║  Player Statistics and Team Possession                                   ║
║                                                                           ║
║  ┌───────────────┬────────────────┬────────────────────────────────────┐ ║
║  │ Team          │ Player ID      │ Distance (m)                       │ ║
║  ├───────────────┼────────────────┼────────────────────────────────────┤ ║
║  │ team_1        │ 2              │ 1234.56                            │ ║
║  │ team_1        │ 5              │ 987.65                             │ ║
║  │ team_1        │ 7              │ 1456.78                            │ ║
║  │ team_1        │ 11             │ 1123.45                            │ ║
║  │ team_2        │ 3              │ 1089.12                            │ ║
║  │ team_2        │ 8              │ 1345.67                            │ ║
║  │ team_2        │ 9              │ 1456.78                            │ ║
║  │ team_2        │ 13             │ 876.54                             │ ║
║  │               │                │                                    │ ║
║  │ Summary - Team Possession Percentage                                │ ║
║  │ Team 1 Possession │            │ 58.23%                            │ ║
║  │ Team 2 Possession │            │ 41.77%                            │ ║
║  └───────────────┴────────────────┴────────────────────────────────────┘ ║
║                                                                           ║
╚═══════════════════════════════════════════════════════════════════════════╝
```

## Tab 3: Video Output View

```
╔═══════════════════════════════════════════════════════════════════════════╗
║ Summary | Data Table | Video Output                                       ║
╠═══════════════════════════════════════════════════════════════════════════╣
║                                                                           ║
║  ┌───────────────────────────────────────────────────────────────────┐   ║
║  │                                                                   │   ║
║  │                                                                   │   ║
║  │                     [VIDEO PLAYBACK AREA]                         │   ║
║  │                                                                   │   ║
║  │              Annotated video with player tracking,                │   ║
║  │              team colors, ball detection, and                     │   ║
║  │              speed/distance overlays                              │   ║
║  │                                                                   │   ║
║  │                                                                   │   ║
║  └───────────────────────────────────────────────────────────────────┘   ║
║                                                                           ║
║  ┌──────────┐  ┌──────────┐                                              ║
║  │  Pause   │  │   Stop   │                                              ║
║  └──────────┘  └──────────┘                                              ║
║                                                                           ║
╚═══════════════════════════════════════════════════════════════════════════╝
```

## Component Relationships

```
MainWindow
    │
    ├─ Input Controls
    │   ├─ Input Video Browse → QFileDialog
    │   └─ Model Browse → QFileDialog
    │
    ├─ Analysis Control
    │   └─ Start Button → launches QProcess (Python)
    │
    ├─ Process Monitor
    │   ├─ Status Label (updates)
    │   └─ Output Console (stdout/stderr)
    │
    └─ Results (QTabWidget)
        │
        ├─ Summary Tab
        │   └─ Message/Status Display
        │
        ├─ Data Table Tab
        │   ├─ Header Label
        │   └─ QTableWidget
        │       ├─ CSV Parser → populate
        │       └─ JSON Parser → populate (fallback)
        │
        └─ Video Tab
            ├─ QVideoWidget
            │   └─ QMediaPlayer (backend)
            │       └─ QAudioOutput
            └─ Controls
                ├─ Play/Pause Button
                └─ Stop Button
```

## Signal/Slot Connections

```
User Interactions:
┌──────────────────────┐      ┌────────────────────────┐
│ Browse Input Button  │─────▶│ onBrowseInputVideo()   │
└──────────────────────┘      └────────────────────────┘

┌──────────────────────┐      ┌────────────────────────┐
│ Browse Model Button  │─────▶│ onBrowseModel()        │
└──────────────────────┘      └────────────────────────┘

┌──────────────────────┐      ┌────────────────────────┐
│ Start Analysis Button│─────▶│ onStartAnalysis()      │
└──────────────────────┘      └────────────────────────┘
                                      │
                                      ▼
                              ┌────────────────────┐
                              │ Launch QProcess    │
                              └────────────────────┘

Process Events:
┌──────────────────────────────┐      ┌──────────────────────────────────┐
│ QProcess::readyReadStdout    │─────▶│ onProcessReadyReadStandardOutput │
└──────────────────────────────┘      └──────────────────────────────────┘

┌──────────────────────────────┐      ┌──────────────────────────────────┐
│ QProcess::readyReadStderr    │─────▶│ onProcessReadyReadStandardError  │
└──────────────────────────────┘      └──────────────────────────────────┘

┌──────────────────────────────┐      ┌────────────────────────────────┐
│ QProcess::finished           │─────▶│ onProcessFinished()            │
└──────────────────────────────┘      └────────────────────────────────┘
                                              │
                                              ▼
                                       ┌──────────────────┐
                                       │ Load CSV         │
                                       │ Load JSON        │
                                       │ Load Video       │
                                       └──────────────────┘

Video Controls:
┌──────────────────────┐      ┌────────────────────────┐
│ Play/Pause Button    │─────▶│ onPlayPauseVideo()     │
└──────────────────────┘      └────────────────────────┘
                                      │
                                      ▼
                              ┌────────────────────┐
                              │ QMediaPlayer       │
                              │ ->play()/pause()   │
                              └────────────────────┘

┌──────────────────────┐      ┌────────────────────────┐
│ Stop Button          │─────▶│ onStopVideo()          │
└──────────────────────┘      └────────────────────────┘
                                      │
                                      ▼
                              ┌────────────────────┐
                              │ QMediaPlayer       │
                              │ ->stop()           │
                              └────────────────────┘
```

## Data Flow Diagram

```
                   ┌─────────────┐
                   │    User     │
                   └──────┬──────┘
                          │ Selects files & starts
                          ▼
              ┌───────────────────────┐
              │    Qt GUI MainWindow  │
              └───────────┬───────────┘
                          │ Launches via QProcess
                          ▼
              ┌───────────────────────┐
              │   Python Analysis     │
              │   (foot-Function)     │
              └───────────┬───────────┘
                          │ Writes files
                          ▼
       ┌──────────────────┴──────────────────┐
       │                                     │
       ▼                                     ▼
┌──────────────┐                    ┌──────────────┐
│ data_output  │                    │ output_video │
│    .csv      │                    │    .avi      │
│    .json     │                    └──────┬───────┘
└──────┬───────┘                           │
       │                                   │
       │ Read & Parse                      │ Load
       ▼                                   ▼
┌────────────────┐               ┌─────────────────┐
│ QTableWidget   │               │ QMediaPlayer    │
│ (Data Table)   │               │ (Video Widget)  │
└────────────────┘               └─────────────────┘
       │                                   │
       └──────────┬────────────────────────┘
                  │ Display
                  ▼
          ┌───────────────┐
          │  Results Tabs │
          └───────────────┘
                  │
                  ▼
          ┌───────────────┐
          │     User      │
          │  Views Results│
          └───────────────┘
```

## File Locations

```
foot/
├── FootAnalysisGUI.pro              [Qt project file]
├── main.cpp                         [App entry point]
├── MainWindow.h                     [Window header]
├── MainWindow.cpp                   [Window implementation]
│
├── foot-Function/                   [Python backend]
│   ├── main.py                      [Python entry]
│   └── output_videos/               [Generated at runtime]
│       ├── output_video.avi         [Annotated video] ──┐
│       ├── data_output.json         [Statistics JSON] ──┤
│       └── data_output.csv          [Statistics CSV]  ──┤
│                                                         │
└── [Build Directory]/                                    │
    └── FootAnalysisGUI.exe                               │
        │                                                 │
        └─ Looks for outputs at:                         │
           ../../foot-Function/output_videos/ ────────────┘
```

## Execution Flow Timeline

```
T=0s    │ User clicks "Start Analysis"
        │   └─ onStartAnalysis() called
        │      ├─ Clear previous results
        │      ├─ Validate inputs
        │      └─ Launch QProcess with Python
        ▼
T=1s    │ Python process starts
        │   └─ stdout/stderr begins flowing
        │      └─ onProcessReadyReadStandardOutput() called repeatedly
        │         └─ Appends text to Analysis Log (real-time)
        ▼
T=2-60s │ Python analysis running
        │   ├─ Reading video frames
        │   ├─ Object detection (YOLO)
        │   ├─ Tracking players
        │   ├─ Team assignment
        │   ├─ Speed/distance calculation
        │   └─ Generating output files
        │
        │ [GUI remains responsive - user can interact]
        ▼
T=60s   │ Python writes output files:
        │   ├─ output_video.avi
        │   ├─ data_output.json
        │   └─ data_output.csv
        ▼
T=61s   │ Python process exits (code 0)
        │   └─ QProcess::finished signal emitted
        │      └─ onProcessFinished() called
        │         ├─ Check exit code
        │         ├─ Call loadAndDisplayCSV()
        │         │  ├─ Read CSV file
        │         │  ├─ Parse into QTableWidget
        │         │  └─ Switch to Data Table tab
        │         ├─ Call loadAndDisplayJSON() [if CSV fails]
        │         └─ Call loadAndPlayVideo()
        │            ├─ Load video into QMediaPlayer
        │            ├─ Enable playback controls
        │            └─ Switch to Video Output tab
        ▼
T=62s   │ Results fully loaded
        │   └─ User can view:
        │      ├─ Data Table (active)
        │      └─ Video Output (ready to play)
        ▼
T=63s+  │ User interacts with results
        │   ├─ Click Play → Video plays
        │   ├─ Switch tabs → View different results
        │   └─ Scroll table → View all players
```
