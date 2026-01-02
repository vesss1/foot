# Video Data Viewer - Code Structure and Flow

## File Organization

```
VideoDataViewer Application
│
├── Main Entry Point
│   └── viewer_main.cpp
│       └── Creates QApplication
│       └── Instantiates VideoDataViewer
│       └── Shows window and runs event loop
│
├── Core Class
│   ├── VideoDataViewer.h
│   │   ├── Class definition
│   │   ├── UI component pointers
│   │   ├── OpenCV VideoCapture
│   │   ├── Data storage structures
│   │   └── Slot declarations
│   │
│   └── VideoDataViewer.cpp
│       ├── Constructor/Destructor
│       ├── setupUI() - Creates all widgets
│       ├── loadVideo() - Opens video with OpenCV
│       ├── loadDataFile() - Routes to CSV/JSON parser
│       ├── updateFrame() - Timer callback for playback
│       ├── displayFrame() - Renders cv::Mat to QLabel
│       └── Control slots (play/pause/reset)
│
└── Configuration
    ├── VideoDataViewer.pro (qmake)
    └── CMakeLists_Viewer.txt (CMake alternative)
```

## Component Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    QMainWindow                              │
│  ┌───────────────────────────────────────────────────────┐  │
│  │              QVBoxLayout (mainLayout)                 │  │
│  │  ┌─────────────────────────────────────────────────┐  │  │
│  │  │         QSplitter (Vertical)                    │  │  │
│  │  │  ┌────────────────────────────────────────────┐ │  │  │
│  │  │  │  QGroupBox (Video Playback)               │ │  │  │
│  │  │  │  ┌──────────────────────────────────────┐ │ │  │  │
│  │  │  │  │  QLabel (videoLabel)                 │ │ │  │  │
│  │  │  │  │  - Displays QPixmap from cv::Mat     │ │ │  │  │
│  │  │  │  │  - Scaled with aspect ratio          │ │ │  │  │
│  │  │  │  └──────────────────────────────────────┘ │ │  │  │
│  │  │  │  ┌──────────────────────────────────────┐ │ │  │  │
│  │  │  │  │  Controls (QHBoxLayout)              │ │ │  │  │
│  │  │  │  │  [Pause] [Reset]    Frame: X / Y    │ │ │  │  │
│  │  │  │  └──────────────────────────────────────┘ │ │  │  │
│  │  │  └────────────────────────────────────────────┘ │  │  │
│  │  │  ┌────────────────────────────────────────────┐ │  │  │
│  │  │  │  QGroupBox (Data Display)                 │ │  │  │
│  │  │  │  ┌──────────────────────────────────────┐ │ │  │  │
│  │  │  │  │  QComboBox (Table/Text switch)       │ │ │  │  │
│  │  │  │  └──────────────────────────────────────┘ │ │  │  │
│  │  │  │  ┌──────────────────────────────────────┐ │ │  │  │
│  │  │  │  │  QTableWidget (dataTable)            │ │ │  │  │
│  │  │  │  │  - Shows parsed CSV/JSON             │ │ │  │  │
│  │  │  │  │  - Sortable columns                  │ │ │  │  │
│  │  │  │  └──────────────────────────────────────┘ │ │  │  │
│  │  │  │  ┌──────────────────────────────────────┐ │ │  │  │
│  │  │  │  │  QTextEdit (dataTextEdit)            │ │ │  │  │
│  │  │  │  │  - Shows raw file content            │ │ │  │  │
│  │  │  │  │  - Initially hidden                  │ │ │  │  │
│  │  │  │  └──────────────────────────────────────┘ │ │  │  │
│  │  │  └────────────────────────────────────────────┘ │  │  │
│  │  └─────────────────────────────────────────────────┘  │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Data Flow

### Video Playback Flow

```
Application Start
    ↓
Constructor
    ↓
setupUI() - Create all widgets
    ↓
Look for: foot-Function/output_videos/output.avi
    ↓
loadVideo(path)
    ├─→ cv::VideoCapture.open(path)
    ├─→ Get total frames, FPS
    ├─→ Read first frame
    └─→ displayFrame(frame)
         ├─→ cv::cvtColor BGR→RGB
         ├─→ Create QImage from cv::Mat
         ├─→ Create QPixmap from QImage
         ├─→ Scale to fit QLabel
         └─→ videoLabel->setPixmap(scaled)
    ↓
Start QTimer at interval = 1000/FPS ms
    ↓
[Timer Loop]
QTimer::timeout()
    ↓
updateFrame()
    ├─→ cv::VideoCapture.read(frame)
    ├─→ displayFrame(frame)
    ├─→ Update frame counter
    └─→ If end of video → seek to start (loop)
```

### Data Loading Flow

```
Application Start
    ↓
Look for data files in output_videos/
    ├─→ Find *.csv files
    └─→ Find *.json files
    ↓
loadDataFile(path)
    ↓
Check extension
    ├─→ .csv → loadCSVData()
    │         ├─→ Read entire file
    │         ├─→ Split by newlines
    │         ├─→ First line = headers
    │         ├─→ Remaining lines = data rows
    │         └─→ Store in QVector<QStringList>
    │
    └─→ .json → loadJSONData()
              ├─→ Read entire file
              ├─→ QJsonDocument::fromJson()
              ├─→ If array of objects:
              │     ├─→ Extract keys as headers
              │     └─→ Extract values as rows
              ├─→ If single object:
              │     ├─→ Create "Key | Value" table
              │     └─→ Each property becomes a row
              └─→ Store in QVector<QStringList>
    ↓
updateDataDisplay()
    ├─→ Populate QTableWidget with headers
    ├─→ Fill table cells with data
    ├─→ Set raw content to QTextEdit
    └─→ Resize columns to fit content
```

## Key Classes and Methods

### VideoDataViewer Class

**Public:**
```cpp
explicit VideoDataViewer(QWidget *parent = nullptr);
~VideoDataViewer();
```

**Private Slots:**
```cpp
void updateFrame();              // Called by QTimer for video playback
void onPlayPauseClicked();       // Toggle play/pause
void onResetClicked();           // Reset video to frame 0
void onDataDisplayModeChanged(); // Switch table/text view
```

**Private Methods:**
```cpp
void setupUI();                  // Create and layout all widgets
bool loadVideo(QString path);    // Open video with OpenCV
bool loadDataFile(QString path); // Load CSV or JSON
void displayFrame(cv::Mat frame);// Render frame to QLabel
QString findDataFile(QString dir);// Auto-detect data file
void loadCSVData(QString path);  // Parse CSV file
void loadJSONData(QString path); // Parse JSON file
void updateDataDisplay();        // Refresh table/text widgets
```

**Member Variables:**
```cpp
// UI Components
QLabel *videoLabel;           // Video display
QTableWidget *dataTable;      // Data table view
QTextEdit *dataTextEdit;      // Raw text view
QPushButton *playPauseButton; // Play/pause control
QLabel *frameInfoLabel;       // Frame counter

// Video playback
cv::VideoCapture videoCapture; // OpenCV video reader
QTimer *frameTimer;            // Frame update timer
cv::Mat currentFrame;          // Current frame buffer
int currentFrameIndex;         // Current position
double fps;                    // Frames per second
bool isPlaying;                // Playback state

// Data storage
QStringList dataHeaders;       // Column headers
QVector<QStringList> dataRows; // Table data
QString dataContent;           // Raw file content
```

## Signal-Slot Connections

```
playPauseButton::clicked → onPlayPauseClicked()
    └─→ Toggle isPlaying flag
    └─→ Update button text

resetButton::clicked → onResetClicked()
    └─→ Seek to frame 0
    └─→ Display first frame

dataDisplayCombo::currentIndexChanged → onDataDisplayModeChanged()
    └─→ Show/hide table or text widget

frameTimer::timeout → updateFrame()
    └─→ Read next frame
    └─→ Display frame
    └─→ Update counter
```

## Build Process

### Using qmake

```
Input: VideoDataViewer.pro
    ↓
qmake generates: Makefile
    ↓
Compiler flags:
    - C++17
    - Qt6 headers (Core, Gui, Widgets)
    - OpenCV headers
    ↓
Link libraries:
    - Qt6::Core
    - Qt6::Gui
    - Qt6::Widgets
    - OpenCV libs (core, imgproc, videoio, etc.)
    ↓
Output: VideoDataViewer executable
```

### Using CMake (Alternative)

```
Input: CMakeLists_Viewer.txt
    ↓
cmake configures project
    ├─→ find_package(Qt6)
    ├─→ find_package(OpenCV)
    ├─→ Enable AUTOMOC for Qt meta-object
    └─→ Generate build files
    ↓
make builds project
    ↓
Output: VideoDataViewer executable
```

## Dependencies

```
VideoDataViewer Application
    │
    ├── Qt 6.10.1
    │   ├── Qt6Core
    │   ├── Qt6Gui
    │   └── Qt6Widgets
    │
    ├── OpenCV 4.x
    │   ├── opencv_core
    │   ├── opencv_imgproc
    │   ├── opencv_videoio
    │   ├── opencv_imgcodecs
    │   └── opencv_highgui (optional)
    │
    └── C++ Standard Library
        ├── <vector>
        ├── <string>
        └── C++17 features
```

## Performance Considerations

1. **Video Decoding**: Done on main thread
   - For large videos, consider QThread
   - Frame buffer could be pre-loaded

2. **Frame Display**: Efficient with QPixmap
   - Scaling done with Qt's hardware acceleration
   - Double buffering automatic

3. **Data Parsing**: One-time at startup
   - CSV: Simple string split O(n)
   - JSON: Qt's parser O(n)
   - Large files (>10MB) may cause startup delay

4. **Memory Usage**:
   - One frame in memory (~2-8MB for HD)
   - All data in memory (QVector)
   - QTableWidget creates items on-demand

## Extension Points

### Adding Video Seeking
```cpp
// Add QSlider for position
QSlider *seekSlider = new QSlider(Qt::Horizontal);
seekSlider->setRange(0, totalFrames - 1);
connect(seekSlider, &QSlider::valueChanged, [this](int frame) {
    videoCapture.set(cv::CAP_PROP_POS_FRAMES, frame);
    videoCapture.read(currentFrame);
    displayFrame(currentFrame);
    currentFrameIndex = frame;
});
```

### Adding Data Filtering
```cpp
// Add QLineEdit for filter
QLineEdit *filterEdit = new QLineEdit();
connect(filterEdit, &QLineEdit::textChanged, [this](const QString &text) {
    for (int row = 0; row < dataTable->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < dataTable->columnCount(); ++col) {
            if (dataTable->item(row, col)->text().contains(text)) {
                match = true;
                break;
            }
        }
        dataTable->setRowHidden(row, !match);
    }
});
```

### Adding Frame Export
```cpp
void VideoDataViewer::exportCurrentFrame() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Save Frame", "", "PNG (*.png);;JPEG (*.jpg)");
    if (!filename.isEmpty()) {
        cv::imwrite(filename.toStdString(), currentFrame);
    }
}
```

## Testing Approach

1. **Unit Testing**: Test individual methods
   - loadCSVData() with sample CSV
   - loadJSONData() with sample JSON
   - Frame conversion (BGR→RGB→QImage)

2. **Integration Testing**: Test full workflow
   - Load video and play
   - Switch data display modes
   - Pause/Resume/Reset

3. **Manual Testing**: Visual verification
   - Video plays smoothly
   - Data displays correctly
   - UI is responsive

## Files Summary

| File | Purpose | Lines | Key Content |
|------|---------|-------|-------------|
| VideoDataViewer.h | Class definition | ~80 | Class declaration, member vars |
| VideoDataViewer.cpp | Implementation | ~450 | All logic and UI setup |
| viewer_main.cpp | Entry point | ~10 | Creates QApplication |
| VideoDataViewer.pro | Build config | ~70 | qmake project file |
| CMakeLists_Viewer.txt | Alt build | ~45 | CMake configuration |
| VIEWER_README.md | Documentation | ~400 | Full documentation |
| USAGE_GUIDE.md | User guide | ~350 | Step-by-step instructions |
| create_test_video.py | Test data | ~60 | Generates test files |
