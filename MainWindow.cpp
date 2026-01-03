#include "MainWindow.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QMovie>
#include <QRegularExpression>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , inputVideoPathEdit(nullptr)
    , browseInputButton(nullptr)
    , modelPathEdit(nullptr)
    , browseModelButton(nullptr)
    , startButton(nullptr)
    , outputTextEdit(nullptr)
    , statusLabel(nullptr)
    , progressBar(nullptr)
    , elapsedTimeLabel(nullptr)
    , elapsedTimer(nullptr)
    , updateTimer(nullptr)
    , resultsTabWidget(nullptr)
    , resultImageLabel(nullptr)
    , resultScrollArea(nullptr)
    , dataTableWidget(nullptr)
    , dataTab(nullptr)
    , mediaPlayer(nullptr)
    , audioOutput(nullptr)
    , videoWidget(nullptr)
    , playPauseButton(nullptr)
    , stopButton(nullptr)
    , videoTab(nullptr)
    , pythonProcess(nullptr)
    , analysisRunning(false)
{
    // Load and apply modern QSS stylesheet
    loadStyleSheet();
    
    setupUI();
    setWindowTitle("Foot Analysis GUI");
    
    // Set reasonable minimum size for window
    setMinimumSize(900, 700);  // Minimum: 900x700 for usability
    
    // Set default size
    resize(1200, 900);
}

MainWindow::~MainWindow()
{
    if (mediaPlayer) {
        mediaPlayer->stop();
        delete mediaPlayer;
    }
    if (audioOutput) {
        delete audioOutput;
    }
    if (pythonProcess) {
        if (pythonProcess->state() == QProcess::Running) {
            pythonProcess->kill();
            pythonProcess->waitForFinished();
        }
        delete pythonProcess;
    }
}

void MainWindow::setupUI()
{
    // Create main splitter for dashboard layout
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setChildrenCollapsible(false);
    setCentralWidget(mainSplitter);
    
    // ===== LEFT SIDEBAR (Fixed ~320px) =====
    QWidget *leftSidebar = new QWidget(this);
    leftSidebar->setProperty("sidebar", true);
    leftSidebar->setMinimumWidth(280);
    leftSidebar->setMaximumWidth(400);
    
    QVBoxLayout *sidebarLayout = new QVBoxLayout(leftSidebar);
    sidebarLayout->setSpacing(16);
    sidebarLayout->setContentsMargins(12, 12, 12, 12);
    
    // Input Configuration Section
    QGroupBox *inputGroup = new QGroupBox("Input Configuration", this);
    inputGroup->setProperty("sidebarCard", true);
    
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
    inputLayout->setSpacing(12);
    inputLayout->setContentsMargins(16, 20, 16, 16);
    
    // Video File
    QLabel *videoLabel = new QLabel("Video File: <span style='color: red;'>*</span>", this);
    inputLayout->addWidget(videoLabel);
    
    QHBoxLayout *videoRowLayout = new QHBoxLayout();
    videoRowLayout->setSpacing(6);
    inputVideoPathEdit = new QLineEdit(this);
    inputVideoPathEdit->setPlaceholderText("Select video file...");
    inputVideoPathEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    browseInputButton = new QToolButton(this);
    browseInputButton->setText("...");
    browseInputButton->setToolTip("Browse for video file");
    browseInputButton->setMinimumSize(28, 28);
    browseInputButton->setMaximumSize(28, 28);
    browseInputButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    videoRowLayout->addWidget(inputVideoPathEdit, 1);
    videoRowLayout->addWidget(browseInputButton, 0);
    inputLayout->addLayout(videoRowLayout);
    
    // YOLO Model
    QLabel *modelLabel = new QLabel("YOLO Model: <span style='color: red;'>*</span>", this);
    inputLayout->addWidget(modelLabel);
    
    QHBoxLayout *modelRowLayout = new QHBoxLayout();
    modelRowLayout->setSpacing(6);
    modelPathEdit = new QLineEdit(this);
    modelPathEdit->setPlaceholderText("Select YOLO model...");
    modelPathEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    browseModelButton = new QToolButton(this);
    browseModelButton->setText("...");
    browseModelButton->setToolTip("Browse for YOLO model");
    browseModelButton->setMinimumSize(28, 28);
    browseModelButton->setMaximumSize(28, 28);
    browseModelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    modelRowLayout->addWidget(modelPathEdit, 1);
    modelRowLayout->addWidget(browseModelButton, 0);
    inputLayout->addLayout(modelRowLayout);
    
    sidebarLayout->addWidget(inputGroup);
    
    // Analysis Control Section
    QGroupBox *controlGroup = new QGroupBox("Analysis Control", this);
    controlGroup->setProperty("sidebarCard", true);
    
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    controlLayout->setSpacing(12);
    controlLayout->setContentsMargins(16, 20, 16, 16);
    
    // Primary CTA - Start Analysis Button
    startButton = new QPushButton("Start Analysis", this);
    startButton->setProperty("primary", true);
    startButton->setMinimumHeight(50);
    startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    controlLayout->addWidget(startButton);
    
    // Progress Bar (hidden initially)
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 0);  // Indeterminate mode
    progressBar->setTextVisible(false);
    progressBar->setMinimumHeight(20);
    progressBar->setVisible(false);  // Hidden initially
    progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    controlLayout->addWidget(progressBar);
    
    // Elapsed Time Label (hidden initially)
    elapsedTimeLabel = new QLabel("Elapsed: 0:00", this);
    elapsedTimeLabel->setProperty("elapsedTime", true);
    elapsedTimeLabel->setAlignment(Qt::AlignCenter);
    elapsedTimeLabel->setVisible(false);  // Hidden initially
    elapsedTimeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    controlLayout->addWidget(elapsedTimeLabel);
    
    sidebarLayout->addWidget(controlGroup);
    
    // Initialize timers
    elapsedTimer = new QElapsedTimer();
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateElapsedTime);
    
    // Status/Progress Section
    QGroupBox *statusGroup = new QGroupBox("Status", this);
    statusGroup->setProperty("sidebarCard", true);
    
    QVBoxLayout *statusGroupLayout = new QVBoxLayout(statusGroup);
    statusGroupLayout->setSpacing(8);
    statusGroupLayout->setContentsMargins(16, 20, 16, 16);
    
    statusLabel = new QLabel("Ready", this);
    statusLabel->setProperty("statusLabel", true);
    statusLabel->setWordWrap(true);
    statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    statusGroupLayout->addWidget(statusLabel);
    
    sidebarLayout->addWidget(statusGroup);
    
    // Add stretch to push everything to the top
    sidebarLayout->addStretch(1);
    
    mainSplitter->addWidget(leftSidebar);
    
    // ===== RIGHT MAIN AREA (TabWidget) =====
    QWidget *mainArea = new QWidget(this);
    QVBoxLayout *mainAreaLayout = new QVBoxLayout(mainArea);
    mainAreaLayout->setSpacing(0);
    mainAreaLayout->setContentsMargins(0, 0, 0, 0);
    
    resultsTabWidget = new QTabWidget(this);
    resultsTabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Tab 1: Summary (with empty state)
    QWidget *summaryTab = new QWidget();
    summaryTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryTab);
    summaryLayout->setContentsMargins(16, 16, 16, 16);
    summaryLayout->setSpacing(0);
    
    resultScrollArea = new QScrollArea(this);
    resultScrollArea->setWidgetResizable(true);
    resultScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    resultScrollArea->setFrameShape(QFrame::NoFrame);
    
    resultImageLabel = new QLabel(this);
    resultImageLabel->setProperty("emptyState", true);
    resultImageLabel->setAlignment(Qt::AlignCenter);
    resultImageLabel->setScaledContents(false);
    resultImageLabel->setText(
        "<div style='text-align: center; color: #666; font-size: 14pt;'>"
        "<p style='font-size: 48pt; margin: 20px;'>📊</p>"
        "<p style='font-weight: bold; margin: 10px;'>No Results Yet</p>"
        "<p style='font-size: 10pt; margin: 5px 10px;'>1. Select a video file</p>"
        "<p style='font-size: 10pt; margin: 5px 10px;'>2. Select a YOLO model</p>"
        "<p style='font-size: 10pt; margin: 5px 10px;'>3. Click \"Start Analysis\"</p>"
        "<p style='font-size: 9pt; color: #999; margin: 15px 10px;'>Results will appear here after analysis completes</p>"
        "</div>"
    );
    resultImageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    resultScrollArea->setWidget(resultImageLabel);
    summaryLayout->addWidget(resultScrollArea);
    resultsTabWidget->addTab(summaryTab, "Summary");
    
    // Tab 2: Data Table
    dataTab = new QWidget();
    dataTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *dataLayout = new QVBoxLayout(dataTab);
    dataLayout->setContentsMargins(16, 16, 16, 16);
    dataLayout->setSpacing(12);
    
    QLabel *dataLabel = new QLabel("Player Statistics and Team Possession", this);
    dataLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    dataLayout->addWidget(dataLabel);
    
    dataTableWidget = new QTableWidget(this);
    dataTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dataTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    dataTableWidget->horizontalHeader()->setStretchLastSection(true);
    dataTableWidget->setAlternatingRowColors(true);
    dataTableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataLayout->addWidget(dataTableWidget);
    
    resultsTabWidget->addTab(dataTab, "Data Table");
    
    // Tab 3: Video Output
    videoTab = new QWidget();
    videoTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *videoLayout = new QVBoxLayout(videoTab);
    videoLayout->setContentsMargins(16, 16, 16, 16);
    videoLayout->setSpacing(12);
    
    videoWidget = new QVideoWidget(this);
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoWidget->setMinimumHeight(300);
    videoLayout->addWidget(videoWidget, 1);
    
    // Video controls
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(8);
    playPauseButton = new QPushButton("Play", this);
    playPauseButton->setEnabled(false);
    playPauseButton->setMinimumWidth(80);
    playPauseButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    stopButton = new QPushButton("Stop", this);
    stopButton->setEnabled(false);
    stopButton->setMinimumWidth(80);
    stopButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    
    controlsLayout->addWidget(playPauseButton);
    controlsLayout->addWidget(stopButton);
    controlsLayout->addStretch();
    
    videoLayout->addLayout(controlsLayout, 0);
    
    resultsTabWidget->addTab(videoTab, "Video Output");
    
    // Tab 4: Logs
    QWidget *logsTab = new QWidget();
    logsTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *logsLayout = new QVBoxLayout(logsTab);
    logsLayout->setContentsMargins(16, 16, 16, 16);
    logsLayout->setSpacing(12);
    
    QHBoxLayout *logsHeaderLayout = new QHBoxLayout();
    QLabel *logsLabel = new QLabel("Analysis Logs", this);
    logsHeaderLayout->addWidget(logsLabel);
    logsHeaderLayout->addStretch();
    
    QPushButton *clearLogButton = new QPushButton("Clear", this);
    clearLogButton->setMinimumWidth(70);
    clearLogButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    logsHeaderLayout->addWidget(clearLogButton);
    
    logsLayout->addLayout(logsHeaderLayout);
    
    outputTextEdit = new QTextEdit(this);
    outputTextEdit->setReadOnly(true);
    outputTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    outputTextEdit->setMinimumHeight(200);
    logsLayout->addWidget(outputTextEdit);
    
    resultsTabWidget->addTab(logsTab, "Logs");
    
    mainAreaLayout->addWidget(resultsTabWidget);
    
    mainSplitter->addWidget(mainArea);
    
    // Set splitter sizes (left sidebar ~320px, rest for main area)
    mainSplitter->setStretchFactor(0, 0);  // Sidebar doesn't stretch
    mainSplitter->setStretchFactor(1, 1);  // Main area stretches
    mainSplitter->setSizes(QList<int>() << 320 << 880);
    
    // Connect clear log button
    connect(clearLogButton, &QPushButton::clicked, [this]() {
        outputTextEdit->clear();
    });
    
    // Initialize media player
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);
    
    // Create status bar
    QStatusBar *statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    
    QLabel *statusBarLabel = new QLabel("Ready", this);
    statusBarLabel->setStyleSheet("padding: 4px; font-size: 9pt;");
    statusBar->addWidget(statusBarLabel);
    
    QLabel *versionLabel = new QLabel("v1.0.0", this);
    versionLabel->setStyleSheet("padding: 4px; font-size: 9pt; color: #666;");
    statusBar->addPermanentWidget(versionLabel);
    
    // Connect signals
    connect(browseInputButton, &QToolButton::clicked, this, &MainWindow::onBrowseInputVideo);
    connect(browseModelButton, &QToolButton::clicked, this, &MainWindow::onBrowseModel);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartAnalysis);
    connect(playPauseButton, &QPushButton::clicked, this, &MainWindow::onPlayPauseVideo);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopVideo);
}

void MainWindow::onBrowseInputVideo()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select Input Video",
        QDir::homePath(),
        "Video Files (*.mp4 *.avi *.mov *.mkv);;All Files (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        inputVideoPathEdit->setText(fileName);
    }
}

void MainWindow::onBrowseModel()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select YOLO Model",
        QDir::homePath(),
        "Model Files (*.pt *.pth);;All Files (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        modelPathEdit->setText(fileName);
    }
}

void MainWindow::onStartAnalysis()
{
    if (analysisRunning) {
        QMessageBox::warning(this, "Analysis Running", "An analysis is already in progress.");
        return;
    }
    
    QString inputVideo = inputVideoPathEdit->text().trimmed();
    QString modelPath = modelPathEdit->text().trimmed();
    
    if (inputVideo.isEmpty()) {
        QMessageBox::warning(this, "Missing Input", "Please select an input video file.");
        return;
    }
    
    if (modelPath.isEmpty()) {
        QMessageBox::warning(this, "Missing Model", "Please select a YOLO model file.");
        return;
    }
    
    if (!QFileInfo::exists(inputVideo)) {
        QMessageBox::critical(this, "File Not Found", "Input video file does not exist.");
        return;
    }
    
    if (!QFileInfo::exists(modelPath)) {
        QMessageBox::critical(this, "File Not Found", "Model file does not exist.");
        return;
    }
    
    // Clear previous results
    outputTextEdit->clear();
    resultImageLabel->clear();
    resultImageLabel->setText("Analysis in progress...");
    dataTableWidget->clearContents();
    dataTableWidget->setRowCount(0);
    dataTableWidget->setColumnCount(0);
    if (mediaPlayer) {
        mediaPlayer->stop();
    }
    playPauseButton->setEnabled(false);
    stopButton->setEnabled(false);
    lastOutputPath.clear();
    
    // Initialize process if needed
    if (!pythonProcess) {
        pythonProcess = new QProcess(this);
        connect(pythonProcess, &QProcess::readyReadStandardOutput, 
                this, &MainWindow::onProcessReadyReadStandardOutput);
        connect(pythonProcess, &QProcess::readyReadStandardError, 
                this, &MainWindow::onProcessReadyReadStandardError);
        connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &MainWindow::onProcessFinished);
    }
    
    // Setup Python command
    QString exeDir = QCoreApplication::applicationDirPath();
    QString scriptPath = QDir(exeDir).absoluteFilePath("../../foot-Function/main.py");
    
    if (!QFileInfo::exists(scriptPath)) {
        QMessageBox::critical(this, "Script Not Found", 
            QString("Python script not found at: %1\n\nMake sure the foot-Function directory is present.").arg(scriptPath));
        return;
    }
    
    QStringList arguments;
    arguments << scriptPath;
    arguments << "--input" << inputVideo;
    arguments << "--model" << modelPath;
    
    // Start the process
    QString workingDir = QDir(exeDir).absoluteFilePath("../../foot-Function");
    pythonProcess->setWorkingDirectory(workingDir);
    pythonProcess->start("python", arguments);
    
    if (!pythonProcess->waitForStarted(3000)) {
        QMessageBox::critical(this, "Process Error", 
            "Failed to start Python process. Make sure Python is installed and in PATH.");
        analysisRunning = false;
        statusLabel->setText("Error: Failed to start");
        return;
    }
    
    analysisRunning = true;
    startButton->setEnabled(false);
    statusLabel->setText("Running analysis...");
    statusLabel->setStyleSheet("color: #0078d4; padding: 12px; border-left: 4px solid #0078d4; border-radius: 4px; background-color: #f0f8ff;");
    
    // Show and start progress indicators
    progressBar->setVisible(true);
    progressBar->setRange(0, 0);  // Indeterminate mode
    
    elapsedTimer->start();
    updateTimer->start(1000);  // Update every second
    elapsedTimeLabel->setVisible(true);
    elapsedTimeLabel->setText("Elapsed: 0:00");
    
    outputTextEdit->append("=== Analysis Started ===\n");
    outputTextEdit->append(QString("Command: python %1\n").arg(arguments.join(" ")));
}

void MainWindow::onProcessReadyReadStandardOutput()
{
    if (pythonProcess) {
        QByteArray data = pythonProcess->readAllStandardOutput();
        QString text = QString::fromUtf8(data);
        outputTextEdit->append(text);
        
        // Auto-scroll to bottom
        QTextCursor cursor = outputTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        outputTextEdit->setTextCursor(cursor);
    }
}

void MainWindow::onProcessReadyReadStandardError()
{
    if (pythonProcess) {
        QByteArray data = pythonProcess->readAllStandardError();
        QString text = QString::fromUtf8(data);
        outputTextEdit->append(QString("<span style='color: red;'>%1</span>").arg(text.toHtmlEscaped()));
        
        // Auto-scroll to bottom
        QTextCursor cursor = outputTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        outputTextEdit->setTextCursor(cursor);
    }
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    analysisRunning = false;
    startButton->setEnabled(true);
    
    // Hide and stop progress indicators
    progressBar->setVisible(false);
    updateTimer->stop();
    elapsedTimeLabel->setVisible(false);
    
    outputTextEdit->append("\n=== Analysis Finished ===\n");
    outputTextEdit->append(QString("Exit Code: %1\n").arg(exitCode));
    
    if (exitStatus == QProcess::CrashExit) {
        statusLabel->setText("✗ Error: Process crashed");
        statusLabel->setStyleSheet("color: #dc3545; padding: 12px; border-left: 4px solid #dc3545; border-radius: 4px; background-color: #fff5f5;");
        QMessageBox::critical(this, "Process Crashed", "The Python process crashed unexpectedly.");
        return;
    }
    
    if (exitCode == 0) {
        statusLabel->setText("✓ Analysis completed successfully");
        statusLabel->setStyleSheet("color: #28a745; padding: 12px; border-left: 4px solid #28a745; border-radius: 4px; background-color: #f0fff4;");
        
        // Get output directory
        // Note: This path is relative to the executable location and assumes the project
        // structure where the executable is in a build directory and foot-Function is at
        // the repository root. This works for the standard Qt Creator build configuration.
        // Path resolution: [build-dir]/[executable] -> ../../foot-Function/output_videos
        QString exeDir = QCoreApplication::applicationDirPath();
        QString outputDirPath = QDir(exeDir).absoluteFilePath("../../foot-Function/output_videos");
        
        // Load CSV data
        QString csvPath = QDir(outputDirPath).absoluteFilePath("data_output.csv");
        if (QFileInfo::exists(csvPath)) {
            loadAndDisplayCSV(csvPath);
            outputTextEdit->append(QString("Loaded CSV data from: %1").arg(csvPath));
        }
        
        // Load JSON data (as fallback if CSV fails)
        QString jsonPath = QDir(outputDirPath).absoluteFilePath("data_output.json");
        if (QFileInfo::exists(jsonPath) && dataTableWidget->rowCount() == 0) {
            loadAndDisplayJSON(jsonPath);
            outputTextEdit->append(QString("Loaded JSON data from: %1").arg(jsonPath));
        }
        
        // Load and play video
        QString videoPath = QDir(outputDirPath).absoluteFilePath("output_video.avi");
        if (QFileInfo::exists(videoPath)) {
            loadAndPlayVideo(videoPath);
            outputTextEdit->append(QString("Loaded video from: %1").arg(videoPath));
        }
        
        // Try to find and display output for summary tab
        QString outputPath = findOutputVideo();
        if (!outputPath.isEmpty()) {
            displayResultMedia(outputPath);
        } else {
            resultImageLabel->setText("Analysis complete!\n\nCheck the Data Table and Video Output tabs to view results.");
        }
    } else {
        statusLabel->setText(QString("✗ Error: Analysis failed (exit code %1)").arg(exitCode));
        statusLabel->setStyleSheet("color: #dc3545; padding: 12px; border-left: 4px solid #dc3545; border-radius: 4px; background-color: #fff5f5;");
        resultImageLabel->setText("Analysis failed. Check the log for error details.");
    }
}

QString MainWindow::findOutputVideo()
{
    // Look for output in the foot-Function/output_videos directory
    QString exeDir = QCoreApplication::applicationDirPath();
    QString outputDirPath = QDir(exeDir).absoluteFilePath("../../foot-Function/output_videos");
    QDir outputDir(outputDirPath);
    
    if (!outputDir.exists()) {
        qDebug() << "Output directory does not exist:" << outputDir.path();
        return QString();
    }
    
    // Look for video files (avi, mp4) or image files (png, jpg)
    QStringList filters;
    filters << "*.avi" << "*.mp4" << "*.png" << "*.jpg" << "*.jpeg";
    
    QFileInfoList files = outputDir.entryInfoList(filters, QDir::Files, QDir::Time);
    
    if (!files.isEmpty()) {
        // Return the most recently modified file
        QString path = files.first().absoluteFilePath();
        qDebug() << "Found output file:" << path;
        return path;
    }
    
    qDebug() << "No output files found in:" << outputDir.path();
    return QString();
}

void MainWindow::displayResultMedia(const QString &mediaPath)
{
    if (mediaPath.isEmpty() || !QFileInfo::exists(mediaPath)) {
        resultImageLabel->setText("Output file not found.");
        return;
    }
    
    lastOutputPath = mediaPath;
    QFileInfo fileInfo(mediaPath);
    QString extension = fileInfo.suffix().toLower();
    
    // For video files, just show a message with the path
    if (extension == "avi" || extension == "mp4" || extension == "mov" || extension == "mkv") {
        resultImageLabel->setText(
            QString("Video analysis complete!\n\n"
                    "Output saved to:\n%1\n\n"
                    "Open this file in your video player to view the results.")
            .arg(mediaPath)
        );
        resultImageLabel->setWordWrap(true);
        return;
    }
    
    // For image files, display the image
    if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "bmp") {
        QPixmap pixmap(mediaPath);
        
        if (pixmap.isNull()) {
            resultImageLabel->setText("Failed to load image.");
            return;
        }
        
        // Scale image to fit while maintaining aspect ratio
        QPixmap scaledPixmap = pixmap.scaled(
            resultScrollArea->viewport()->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        
        resultImageLabel->setProperty("emptyState", false);  // Remove empty state property
        resultImageLabel->setPixmap(scaledPixmap);
        resultImageLabel->setText("");
        return;
    }
    
    // Unknown file type
    resultImageLabel->setProperty("emptyState", false);  // Remove empty state property
    resultImageLabel->setText(
        QString("Analysis complete!\n\nOutput saved to:\n%1").arg(mediaPath)
    );
}

void MainWindow::loadAndDisplayCSV(const QString &csvPath)
{
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open CSV file:" << csvPath;
        return;
    }
    
    QTextStream in(&file);
    QStringList rows;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.trimmed().isEmpty()) {
            rows.append(line);
        }
    }
    file.close();
    
    if (rows.isEmpty()) {
        qDebug() << "CSV file is empty";
        return;
    }
    
    // Parse CSV - handle basic comma separation
    // Note: This expects simple CSV without quoted fields (as generated by Python script)
    QStringList headers = rows[0].split(',');
    dataTableWidget->setColumnCount(headers.size());
    dataTableWidget->setHorizontalHeaderLabels(headers);
    
    // Set rows
    dataTableWidget->setRowCount(rows.size() - 1);
    
    for (int i = 1; i < rows.size(); ++i) {
        QStringList columns = rows[i].split(',');
        for (int j = 0; j < columns.size() && j < headers.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(columns[j].trimmed());
            dataTableWidget->setItem(i - 1, j, item);
        }
    }
    
    // Resize columns to content
    dataTableWidget->resizeColumnsToContents();
    
    // Switch to data tab
    resultsTabWidget->setCurrentWidget(dataTab);
}

void MainWindow::loadAndDisplayJSON(const QString &jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file:" << jsonPath;
        return;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Invalid JSON format";
        return;
    }
    
    QJsonObject root = doc.object();
    
    // Setup table headers
    dataTableWidget->setColumnCount(3);
    dataTableWidget->setHorizontalHeaderLabels(QStringList() << "Team" << "Player ID" << "Distance (m)");
    
    int row = 0;
    
    // Process each team
    for (const QString &key : root.keys()) {
        if (key == "summary") {
            continue;
        }
        
        QJsonObject teamData = root[key].toObject();
        for (const QString &playerId : teamData.keys()) {
            QJsonObject playerData = teamData[playerId].toObject();
            double distanceM = playerData["distance_m"].toDouble();
            
            dataTableWidget->insertRow(row);
            dataTableWidget->setItem(row, 0, new QTableWidgetItem(key));
            dataTableWidget->setItem(row, 1, new QTableWidgetItem(playerId));
            dataTableWidget->setItem(row, 2, new QTableWidgetItem(
                distanceM == 0 ? "Not Detected" : QString::number(distanceM, 'f', 2)
            ));
            row++;
        }
    }
    
    // Add summary rows
    if (root.contains("summary")) {
        QJsonObject summary = root["summary"].toObject();
        
        // Add empty row for separation
        dataTableWidget->insertRow(row);
        row++;
        
        // Add summary header
        dataTableWidget->insertRow(row);
        QTableWidgetItem *headerItem = new QTableWidgetItem("Summary - Team Possession Percentage");
        QFont boldFont;
        boldFont.setBold(true);
        headerItem->setFont(boldFont);
        dataTableWidget->setItem(row, 0, headerItem);
        row++;
        
        // Add possession percentages
        if (summary.contains("team_1_possession_percent")) {
            dataTableWidget->insertRow(row);
            dataTableWidget->setItem(row, 0, new QTableWidgetItem("Team 1 Possession"));
            dataTableWidget->setItem(row, 2, new QTableWidgetItem(
                QString::number(summary["team_1_possession_percent"].toDouble(), 'f', 2) + "%"
            ));
            row++;
        }
        
        if (summary.contains("team_2_possession_percent")) {
            dataTableWidget->insertRow(row);
            dataTableWidget->setItem(row, 0, new QTableWidgetItem("Team 2 Possession"));
            dataTableWidget->setItem(row, 2, new QTableWidgetItem(
                QString::number(summary["team_2_possession_percent"].toDouble(), 'f', 2) + "%"
            ));
            row++;
        }
    }
    
    // Resize columns to content
    dataTableWidget->resizeColumnsToContents();
    
    // Switch to data tab
    resultsTabWidget->setCurrentWidget(dataTab);
}

void MainWindow::loadAndPlayVideo(const QString &videoPath)
{
    if (!QFileInfo::exists(videoPath)) {
        qDebug() << "Video file does not exist:" << videoPath;
        return;
    }
    
    mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));
    playPauseButton->setEnabled(true);
    stopButton->setEnabled(true);
    
    // Switch to video tab
    resultsTabWidget->setCurrentWidget(videoTab);
}

void MainWindow::onPlayPauseVideo()
{
    if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        mediaPlayer->pause();
        playPauseButton->setText("Play");
    } else {
        mediaPlayer->play();
        playPauseButton->setText("Pause");
    }
}

void MainWindow::onStopVideo()
{
    mediaPlayer->stop();
    playPauseButton->setText("Play");
}

void MainWindow::updateElapsedTime()
{
    if (elapsedTimer->isValid()) {
        qint64 elapsed = elapsedTimer->elapsed();  // milliseconds
        int seconds = elapsed / 1000;
        int minutes = seconds / 60;
        seconds = seconds % 60;
        
        QString timeText = QString("Elapsed: %1:%2")
            .arg(minutes)
            .arg(seconds, 2, 10, QChar('0'));
        elapsedTimeLabel->setText(timeText);
    }
}

void MainWindow::loadStyleSheet()
{
    // Load modern QSS stylesheet
    QFile styleFile(":/modern_style.qss");
    
    // If resource not found, try file system
    if (!styleFile.exists()) {
        styleFile.setFileName("modern_style.qss");
    }
    
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        QApplication::instance()->setStyleSheet(styleSheet);
        styleFile.close();
        qDebug() << "Modern stylesheet loaded successfully";
    } else {
        qDebug() << "Failed to load stylesheet. Using default Qt styling.";
    }
}
