/*******************************************************************************
 * MAIN WINDOW HEADER
 * 
 * This header defines the MainWindow class, which provides the primary GUI
 * for the Football Analysis application. The window integrates Qt widgets
 * with a Python-based video analysis backend.
 * 
 * KEY RESPONSIBILITIES:
 * - User interface for selecting input video and YOLO model
 * - Asynchronous execution of Python analysis pipeline via QProcess
 * - Real-time display of analysis progress and log output
 * - Automatic loading and visualization of analysis results (CSV/JSON tables)
 * - Embedded video player for viewing annotated output videos
 * 
 * ARCHITECTURE:
 * The MainWindow acts as a bridge between the Qt GUI and Python backend:
 *   User Input → Qt GUI → QProcess → Python main.py → Output Files → Qt Display
 * 
 * INCLUDES:
 * - Qt Core: Main window framework, process management, timers
 * - Qt Widgets: All UI components (buttons, text, tables, tabs)
 * - Qt Multimedia: Video playback with media player and video widget
 ******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QPixmap>
#include <QGroupBox>
#include <QTableWidget>
#include <QTabWidget>
#include <QHeaderView>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtMultimedia/QAudioOutput>
#include <QSplitter>
#include <QStatusBar>
#include <QToolButton>
#include <QProgressBar>
#include <QElapsedTimer>
#include <QTimer>

/**
 * @class MainWindow
 * @brief Primary application window for Football Analysis GUI
 * 
 * Provides a complete interface for:
 * - Configuring analysis parameters (video input, model selection)
 * - Running Python-based video analysis asynchronously
 * - Monitoring analysis progress in real-time
 * - Displaying results in tabbed interface (summary, data table, video player)
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ===== EVENT HANDLERS: User Interactions =====
    void onBrowseInputVideo();     // Open file dialog to select input video
    void onBrowseModel();          // Open file dialog to select YOLO model
    void onStartAnalysis();        // Launch Python analysis process
    
    // ===== EVENT HANDLERS: Process Communication =====
    void onProcessReadyReadStandardOutput();  // Capture Python stdout in real-time
    void onProcessReadyReadStandardError();   // Capture Python stderr in real-time
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);  // Handle completion, load results
    
    // ===== EVENT HANDLERS: Video Playback =====
    void onPlayPauseVideo();       // Toggle video play/pause
    void onStopVideo();            // Stop video and reset to beginning
    void updateElapsedTime();      // Update elapsed time display (timer callback)

private:
    // ===== UI SETUP METHODS =====
    void setupUI();                // Construct all UI widgets and layouts
    void loadStyleSheet();         // Load and apply QSS stylesheet
    
    // ===== RESULT LOADING METHODS =====
    void displayResultMedia(const QString &mediaPath);      // Legacy method for displaying results
    QString findOutputVideo();                              // Locate output video file
    void loadAndDisplayCSV(const QString &csvPath);         // Parse and display CSV data in table
    void loadAndDisplayJSON(const QString &jsonPath);       // Parse and display JSON data in table
    void loadAndPlayVideo(const QString &videoPath);        // Load video into media player
    
    // ===== UTILITY METHODS =====
    QString getProjectRootPath() const;  // Get absolute path to project root
    
    // ===== UI COMPONENTS: Layout =====
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    
    // ===== UI COMPONENTS: Input Controls =====
    QLineEdit *inputVideoPathEdit;      // Text field showing selected input video path
    QToolButton *browseInputButton;     // Button to browse for input video
    QLineEdit *modelPathEdit;           // Text field showing selected model path
    QToolButton *browseModelButton;     // Button to browse for YOLO model
    QPushButton *startButton;           // Button to start analysis
    
    // ===== UI COMPONENTS: Progress Display =====
    QTextEdit *outputTextEdit;          // Log output from Python process (stdout/stderr)
    QLabel *statusLabel;                // Current status message
    QProgressBar *progressBar;          // Visual progress indicator
    QLabel *elapsedTimeLabel;           // Elapsed time counter
    QElapsedTimer *elapsedTimer;        // Timer for measuring elapsed time
    QTimer *updateTimer;                // Timer for periodic UI updates
    
    // ===== UI COMPONENTS: Results Tabs =====
    QTabWidget *resultsTabWidget;       // Tab container (Summary, Data Table, Video Output)
    QLabel *resultImageLabel;           // Label for displaying result images (legacy)
    QScrollArea *resultScrollArea;      // Scroll area for large content
    
    // ===== UI COMPONENTS: Data Display (CSV/JSON) =====
    QTableWidget *dataTableWidget;      // Table widget for displaying player statistics
    QWidget *dataTab;                   // Container widget for data table tab
    
    // ===== UI COMPONENTS: Video Playback =====
    QMediaPlayer *mediaPlayer;          // Qt Multimedia player for video playback
    QAudioOutput *audioOutput;          // Audio output device (attached to media player)
    QVideoWidget *videoWidget;          // Video rendering widget
    QPushButton *playPauseButton;       // Play/pause toggle button
    QPushButton *stopButton;            // Stop button
    QWidget *videoTab;                  // Container widget for video playback tab
    
    // ===== PROCESS MANAGEMENT =====
    QProcess *pythonProcess;            // QProcess for running Python analysis asynchronously
    
    // ===== APPLICATION STATE =====
    QString lastOutputPath;             // Path to most recent output directory
    bool analysisRunning;               // Flag indicating if analysis is currently running
};

#endif // MAINWINDOW_H
