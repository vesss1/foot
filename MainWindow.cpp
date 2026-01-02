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
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Input Video Section
    QGroupBox *inputVideoGroup = new QGroupBox("Input Video", this);
    QHBoxLayout *inputVideoLayout = new QHBoxLayout(inputVideoGroup);
    inputVideoPathEdit = new QLineEdit(this);
    inputVideoPathEdit->setPlaceholderText("Select input video file...");
    browseInputButton = new QPushButton("Browse...", this);
    inputVideoLayout->addWidget(inputVideoPathEdit);
    inputVideoLayout->addWidget(browseInputButton);
    mainLayout->addWidget(inputVideoGroup);
    
    // Model Section
    QGroupBox *modelGroup = new QGroupBox("YOLO Model", this);
    QHBoxLayout *modelLayout = new QHBoxLayout(modelGroup);
    modelPathEdit = new QLineEdit(this);
    modelPathEdit->setPlaceholderText("Select YOLO model file...");
    browseModelButton = new QPushButton("Browse...", this);
    modelLayout->addWidget(modelPathEdit);
    modelLayout->addWidget(browseModelButton);
    mainLayout->addWidget(modelGroup);
    
    // Start Button
    startButton = new QPushButton("Start Analysis", this);
    startButton->setMinimumHeight(40);
    mainLayout->addWidget(startButton);
    
    // Status Label
    statusLabel = new QLabel("Ready", this);
    statusLabel->setStyleSheet("QLabel { padding: 5px; background-color: #f0f0f0; }");
    mainLayout->addWidget(statusLabel);
    
    // Output Log Section
    QGroupBox *outputGroup = new QGroupBox("Analysis Log", this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
    outputTextEdit = new QTextEdit(this);
    outputTextEdit->setReadOnly(true);
    outputTextEdit->setMinimumHeight(150);
    outputLayout->addWidget(outputTextEdit);
    mainLayout->addWidget(outputGroup);
    
    // Results Section with Tabs
    resultsGroupBox = new QGroupBox("Analysis Results", this);
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroupBox);
    
    resultsTabWidget = new QTabWidget(this);
    
    // Tab 1: Summary/Image View
    QWidget *summaryTab = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryTab);
    
    resultScrollArea = new QScrollArea(this);
    resultScrollArea->setWidgetResizable(true);
    resultScrollArea->setMinimumHeight(200);
    
    resultImageLabel = new QLabel(this);
    resultImageLabel->setAlignment(Qt::AlignCenter);
    resultImageLabel->setScaledContents(false);
    resultImageLabel->setText("Results will appear here after analysis completes.");
    resultImageLabel->setStyleSheet("QLabel { background-color: #e0e0e0; padding: 20px; }");
    
    resultScrollArea->setWidget(resultImageLabel);
    summaryLayout->addWidget(resultScrollArea);
    resultsTabWidget->addTab(summaryTab, "Summary");
    
    // Tab 2: Data Table (CSV/JSON)
    dataTab = new QWidget();
    QVBoxLayout *dataLayout = new QVBoxLayout(dataTab);
    
    QLabel *dataLabel = new QLabel("Player Statistics and Team Possession", this);
    dataLabel->setStyleSheet("QLabel { font-weight: bold; padding: 5px; }");
    dataLayout->addWidget(dataLabel);
    
    dataTableWidget = new QTableWidget(this);
    dataTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dataTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    dataTableWidget->horizontalHeader()->setStretchLastSection(true);
    dataTableWidget->setAlternatingRowColors(true);
    dataLayout->addWidget(dataTableWidget);
    
    resultsTabWidget->addTab(dataTab, "Data Table");
    
    // Tab 3: Video Player
    videoTab = new QWidget();
    QVBoxLayout *videoLayout = new QVBoxLayout(videoTab);
    
    videoWidget = new QVideoWidget(this);
    videoWidget->setMinimumHeight(300);
    videoWidget->setStyleSheet("background-color: black;");
    videoLayout->addWidget(videoWidget);
    
    // Video controls
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    playPauseButton = new QPushButton("Play", this);
    playPauseButton->setEnabled(false);
    stopButton = new QPushButton("Stop", this);
    stopButton->setEnabled(false);
    
    controlsLayout->addWidget(playPauseButton);
    controlsLayout->addWidget(stopButton);
    controlsLayout->addStretch();
    
    videoLayout->addLayout(controlsLayout);
    
    resultsTabWidget->addTab(videoTab, "Video Output");
    
    resultsLayout->addWidget(resultsTabWidget);
    mainLayout->addWidget(resultsGroupBox);
    
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
    
    // Parse CSV
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
