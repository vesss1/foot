#include "MainWindow.h"
#include <QCoreApplication>
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
    , resultsTabWidget(nullptr)
    , resultImageLabel(nullptr)
    , resultScrollArea(nullptr)
    , resultsGroupBox(nullptr)
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
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create a scroll area for the entire content
    QScrollArea *mainScrollArea = new QScrollArea(this);
    mainScrollArea->setWidgetResizable(true);
    mainScrollArea->setFrameShape(QFrame::NoFrame);
    setCentralWidget(mainScrollArea);
    
    // Content widget inside scroll area
    QWidget *contentWidget = new QWidget();
    mainScrollArea->setWidget(contentWidget);
    
    // Main grid layout: 2 columns
    QGridLayout *mainGrid = new QGridLayout(contentWidget);
    mainGrid->setSpacing(20);  // Block spacing: 20px for visual rhythm
    mainGrid->setContentsMargins(16, 16, 16, 16);
    mainGrid->setColumnStretch(0, 1);  // Left column
    mainGrid->setColumnStretch(1, 1);  // Right column
    
    int currentRow = 0;
    
    // ===== LEFT COLUMN =====
    
    // Input Configuration Group (Left)
    QGroupBox *inputGroup = new QGroupBox("Input Configuration", this);
    inputGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // GroupBox title styling: 11pt, bold
    inputGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
    
    QFormLayout *inputFormLayout = new QFormLayout(inputGroup);
    inputFormLayout->setSpacing(10);  // Field spacing: 10px for consistency
    inputFormLayout->setContentsMargins(16, 16, 16, 16);
    inputFormLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    inputFormLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    
    // Video File Row (Required field)
    QHBoxLayout *videoRowLayout = new QHBoxLayout();
    videoRowLayout->setSpacing(8);
    inputVideoPathEdit = new QLineEdit(this);
    inputVideoPathEdit->setPlaceholderText("Select input video file...");
    inputVideoPathEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    browseInputButton = new QPushButton("Browse...", this);
    browseInputButton->setMinimumWidth(90);
    browseInputButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    videoRowLayout->addWidget(inputVideoPathEdit, 1);
    videoRowLayout->addWidget(browseInputButton, 0);
    
    // Create label with required marker
    QLabel *videoLabel = new QLabel("Video File: <span style='color: red;'>*</span>", this);
    videoLabel->setStyleSheet("font-size: 10pt;");  // Field label: 10pt
    inputFormLayout->addRow(videoLabel, videoRowLayout);
    
    // YOLO Model Row (Required field)
    QHBoxLayout *modelRowLayout = new QHBoxLayout();
    modelRowLayout->setSpacing(8);
    modelPathEdit = new QLineEdit(this);
    modelPathEdit->setPlaceholderText("Select YOLO model file...");
    modelPathEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    browseModelButton = new QPushButton("Browse...", this);
    browseModelButton->setMinimumWidth(90);
    browseModelButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    modelRowLayout->addWidget(modelPathEdit, 1);
    modelRowLayout->addWidget(browseModelButton, 0);
    
    // Create label with required marker
    QLabel *modelLabel = new QLabel("YOLO Model: <span style='color: red;'>*</span>", this);
    modelLabel->setStyleSheet("font-size: 10pt;");  // Field label: 10pt
    inputFormLayout->addRow(modelLabel, modelRowLayout);
    
    mainGrid->addWidget(inputGroup, currentRow, 0);
    
    // Analysis Control Group (Right)
    QGroupBox *controlGroup = new QGroupBox("Analysis Control", this);
    controlGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // GroupBox title styling: 11pt, bold
    controlGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
    
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    controlLayout->setSpacing(10);  // Field spacing: 10px for consistency
    controlLayout->setContentsMargins(16, 16, 16, 16);
    
    startButton = new QPushButton("Start Analysis", this);
    startButton->setMinimumHeight(40);
    startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    controlLayout->addWidget(startButton);
    
    mainGrid->addWidget(controlGroup, currentRow, 1);
    
    currentRow++;
    
    // Add spacing between sections (removed setRowMinimumHeight, using mainGrid spacing instead)
    currentRow++;
    
    // ===== FULL WIDTH SECTIONS =====
    
    // Output Log Section (Full Width) - Restructured with Summary/Details/Actions
    QWidget *logContainer = new QWidget(this);
    logContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *logContainerLayout = new QVBoxLayout(logContainer);
    logContainerLayout->setSpacing(10);  // Spacing between log subsections: 10px
    logContainerLayout->setContentsMargins(0, 0, 0, 0);
    
    // Summary Section - Fixed height (3-5 lines)
    QGroupBox *summaryGroup = new QGroupBox("Summary", this);
    summaryGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // GroupBox title styling: 11pt, bold
    summaryGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
    
    QVBoxLayout *summaryGroupLayout = new QVBoxLayout(summaryGroup);
    summaryGroupLayout->setSpacing(8);
    summaryGroupLayout->setContentsMargins(16, 16, 16, 16);
    
    statusLabel = new QLabel("Ready", this);
    statusLabel->setStyleSheet("QLabel { padding: 8px; background-color: #f0f0f0; border-radius: 3px; font-size: 10pt; }");
    statusLabel->setWordWrap(true);
    statusLabel->setMaximumHeight(80);  // Fixed height for 3-5 lines
    statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    summaryGroupLayout->addWidget(statusLabel);
    
    logContainerLayout->addWidget(summaryGroup);
    
    // Details Section - Scrollable log area
    QGroupBox *detailsGroup = new QGroupBox("Details", this);
    detailsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // GroupBox title styling: 11pt, bold
    detailsGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
    
    QVBoxLayout *detailsGroupLayout = new QVBoxLayout(detailsGroup);
    detailsGroupLayout->setSpacing(0);
    detailsGroupLayout->setContentsMargins(16, 16, 16, 16);
    
    outputTextEdit = new QTextEdit(this);
    outputTextEdit->setReadOnly(true);
    outputTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    outputTextEdit->setMinimumHeight(100);  // Reasonable minimum
    outputTextEdit->setStyleSheet("font-size: 9pt;");  // Content text: 9pt
    detailsGroupLayout->addWidget(outputTextEdit);
    
    logContainerLayout->addWidget(detailsGroup);
    
    // Actions Section - Buttons right-aligned at bottom
    QGroupBox *actionsGroup = new QGroupBox("Actions", this);
    actionsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // GroupBox title styling: 11pt, bold
    actionsGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
    
    QVBoxLayout *actionsGroupLayout = new QVBoxLayout(actionsGroup);
    actionsGroupLayout->setSpacing(8);
    actionsGroupLayout->setContentsMargins(16, 16, 16, 16);
    
    QHBoxLayout *actionsButtonLayout = new QHBoxLayout();
    actionsButtonLayout->setSpacing(8);
    actionsButtonLayout->addStretch();  // Push buttons to the right
    
    QPushButton *clearLogButton = new QPushButton("Clear Log", this);
    clearLogButton->setMinimumWidth(90);
    clearLogButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    actionsButtonLayout->addWidget(clearLogButton);
    
    actionsGroupLayout->addLayout(actionsButtonLayout);
    
    logContainerLayout->addWidget(actionsGroup);
    
    mainGrid->addWidget(logContainer, currentRow, 0, 1, 2);  // span 2 columns
    
    // Connect clear log button
    connect(clearLogButton, &QPushButton::clicked, [this]() {
        outputTextEdit->clear();
    });
    
    currentRow++;
    
    // Add spacing between sections (removed setRowMinimumHeight, using mainGrid spacing instead)
    currentRow++;
    
    // Results Section with Tabs (Full Width)
    resultsGroupBox = new QGroupBox("Analysis Results", this);
    resultsGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // GroupBox title styling: 11pt, bold
    resultsGroupBox->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
    
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroupBox);
    resultsLayout->setSpacing(0);
    resultsLayout->setContentsMargins(16, 16, 16, 16);
    
    resultsTabWidget = new QTabWidget(this);
    resultsTabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Tab 1: Summary/Image View
    QWidget *summaryTab = new QWidget();
    summaryTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryTab);
    summaryLayout->setContentsMargins(5, 5, 5, 5);
    summaryLayout->setSpacing(0);
    
    resultScrollArea = new QScrollArea(this);
    resultScrollArea->setWidgetResizable(true);
    resultScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    resultImageLabel = new QLabel(this);
    resultImageLabel->setAlignment(Qt::AlignCenter);
    resultImageLabel->setScaledContents(false);
    resultImageLabel->setText("Results will appear here after analysis completes.");
    resultImageLabel->setStyleSheet("QLabel { background-color: #e0e0e0; padding: 20px; }");
    resultImageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    resultScrollArea->setWidget(resultImageLabel);
    summaryLayout->addWidget(resultScrollArea);
    resultsTabWidget->addTab(summaryTab, "Summary");
    
    // Tab 2: Data Table (CSV/JSON)
    dataTab = new QWidget();
    dataTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *dataLayout = new QVBoxLayout(dataTab);
    dataLayout->setContentsMargins(5, 5, 5, 5);
    dataLayout->setSpacing(8);
    
    QLabel *dataLabel = new QLabel("Player Statistics and Team Possession", this);
    dataLabel->setStyleSheet("QLabel { font-weight: bold; padding: 5px; }");
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
    
    // Tab 3: Video Player
    videoTab = new QWidget();
    videoTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *videoLayout = new QVBoxLayout(videoTab);
    videoLayout->setContentsMargins(5, 5, 5, 5);
    videoLayout->setSpacing(8);
    
    videoWidget = new QVideoWidget(this);
    videoWidget->setStyleSheet("background-color: black;");
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoWidget->setMinimumHeight(200);  // Reasonable minimum for video playback
    videoLayout->addWidget(videoWidget, 1);
    
    // Video controls
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(8);
    playPauseButton = new QPushButton("Play", this);
    playPauseButton->setEnabled(false);
    playPauseButton->setMinimumWidth(90);
    playPauseButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    stopButton = new QPushButton("Stop", this);
    stopButton->setEnabled(false);
    stopButton->setMinimumWidth(90);
    stopButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    
    controlsLayout->addWidget(playPauseButton);
    controlsLayout->addWidget(stopButton);
    controlsLayout->addStretch();
    
    videoLayout->addLayout(controlsLayout, 0);
    
    resultsTabWidget->addTab(videoTab, "Video Output");
    
    resultsLayout->addWidget(resultsTabWidget);
    mainGrid->addWidget(resultsGroupBox, currentRow, 0, 1, 2);  // span 2 columns
    
    currentRow++;
    
    // Add spacer at the bottom to push content up
    mainGrid->setRowStretch(currentRow, 1);
    
    // Initialize media player
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);
    
    // Connect signals
    connect(browseInputButton, &QPushButton::clicked, this, &MainWindow::onBrowseInputVideo);
    connect(browseModelButton, &QPushButton::clicked, this, &MainWindow::onBrowseModel);
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
    statusLabel->setText("Status: Running analysis...");
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
    
    outputTextEdit->append("\n=== Analysis Finished ===\n");
    outputTextEdit->append(QString("Exit Code: %1\n").arg(exitCode));
    
    if (exitStatus == QProcess::CrashExit) {
        statusLabel->setText("Status: Process crashed");
        QMessageBox::critical(this, "Process Crashed", "The Python process crashed unexpectedly.");
        return;
    }
    
    if (exitCode == 0) {
        statusLabel->setText("Status: Analysis completed successfully");
        
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
        statusLabel->setText(QString("Status: Analysis failed (exit code %1)").arg(exitCode));
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
        
        resultImageLabel->setPixmap(scaledPixmap);
        resultImageLabel->setText("");
        return;
    }
    
    // Unknown file type
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
