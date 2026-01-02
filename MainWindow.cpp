#include "MainWindow.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QMovie>
#include <QRegularExpression>
#include <QDebug>

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
    , resultImageLabel(nullptr)
    , resultScrollArea(nullptr)
    , resultsGroupBox(nullptr)
    , pythonProcess(nullptr)
    , analysisRunning(false)
{
    setupUI();
    setWindowTitle("Foot Analysis GUI");
    resize(1000, 800);
}

MainWindow::~MainWindow()
{
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
    outputTextEdit->setMinimumHeight(200);
    outputLayout->addWidget(outputTextEdit);
    mainLayout->addWidget(outputGroup);
    
    // Results Section
    resultsGroupBox = new QGroupBox("Analysis Results", this);
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroupBox);
    
    resultScrollArea = new QScrollArea(this);
    resultScrollArea->setWidgetResizable(true);
    resultScrollArea->setMinimumHeight(250);
    
    resultImageLabel = new QLabel(this);
    resultImageLabel->setAlignment(Qt::AlignCenter);
    resultImageLabel->setScaledContents(false);
    resultImageLabel->setText("Results will appear here after analysis completes.");
    resultImageLabel->setStyleSheet("QLabel { background-color: #e0e0e0; padding: 20px; }");
    
    resultScrollArea->setWidget(resultImageLabel);
    resultsLayout->addWidget(resultScrollArea);
    mainLayout->addWidget(resultsGroupBox);
    
    // Connect signals
    connect(browseInputButton, &QPushButton::clicked, this, &MainWindow::onBrowseInputVideo);
    connect(browseModelButton, &QPushButton::clicked, this, &MainWindow::onBrowseModel);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartAnalysis);
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
    QString pythonScript = QCoreApplication::applicationDirPath() + "/../foot-Function/main.py";
    
    if (!QFileInfo::exists(pythonScript)) {
        QMessageBox::critical(this, "Script Not Found", 
            QString("Python script not found at: %1\n\nMake sure the foot-Function directory is present.").arg(pythonScript));
        return;
    }
    
    QStringList arguments;
    arguments << pythonScript;
    arguments << "--input" << inputVideo;
    arguments << "--model" << modelPath;
    
    // Start the process
    pythonProcess->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/../foot-Function");
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
        
        // Try to find and display output
        QString outputPath = findOutputVideo();
        if (!outputPath.isEmpty()) {
            displayResultMedia(outputPath);
        } else {
            resultImageLabel->setText("Analysis complete, but output file not found.\nCheck the log for details.");
        }
    } else {
        statusLabel->setText(QString("Status: Analysis failed (exit code %1)").arg(exitCode));
        resultImageLabel->setText("Analysis failed. Check the log for error details.");
    }
}

QString MainWindow::findOutputVideo()
{
    // Look for output in the foot-Function/output_videos directory
    QDir outputDir(QCoreApplication::applicationDirPath() + "/../foot-Function/output_videos");
    
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
