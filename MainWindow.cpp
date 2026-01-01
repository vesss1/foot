#include "MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QTemporaryFile>
#include <QCoreApplication>
#include <QStatusBar>
#include <QScrollBar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_process(nullptr)
    , m_mediaPlayer(nullptr)
    , m_audioOutput(nullptr)
{
    // Determine foot-Function path by searching up from application directory
    QString appDir = QCoreApplication::applicationDirPath();
    QDir searchDir(appDir);
    
    // Search for foot-Function in current dir and parent directories (up to 5 levels)
    bool found = false;
    for (int i = 0; i < 5 && !found; ++i) {
        if (searchDir.exists("foot-Function")) {
            QDir footFuncDir = searchDir;
            if (footFuncDir.cd("foot-Function") && QFileInfo(footFuncDir.filePath("main.py")).exists()) {
                m_footFunctionPath = searchDir.filePath("foot-Function");
                found = true;
                break;
            }
        }
        if (!searchDir.cdUp()) {
            break;
        }
    }
    
    // If not found, try current working directory
    if (!found) {
        QDir cwdDir(QDir::currentPath());
        if (cwdDir.exists("foot-Function")) {
            QDir footFuncDir = cwdDir;
            if (footFuncDir.cd("foot-Function") && QFileInfo(footFuncDir.filePath("main.py")).exists()) {
                m_footFunctionPath = cwdDir.filePath("foot-Function");
                found = true;
            }
        }
    }
    
    // If still not found, set a default and let validation handle it
    if (!found) {
        m_footFunctionPath = QDir(appDir).filePath("foot-Function");
    }
    
    setupUI();
    
    // Initialize process
    m_process = new QProcess(this);
    
    // Connect process signals
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onProcessReadyReadStdout);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &MainWindow::onProcessReadyReadStderr);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &MainWindow::onProcessError);
    connect(m_process, &QProcess::stateChanged,
            this, &MainWindow::onProcessStateChanged);
    
    // Set default values
    m_inputVideoEdit->setText(QDir(m_footFunctionPath).filePath("input_videos/08fd33_4.mp4"));
    m_modelFileEdit->setText(QDir(m_footFunctionPath).filePath("models/best.pt"));
    m_outputDirEdit->setText(QDir(m_footFunctionPath).filePath("output_videos"));
    m_useStubsCheckbox->setChecked(true);
    
    appendOutput("Football Analysis GUI initialized.", false);
    appendOutput("foot-Function path: " + m_footFunctionPath, false);
    
    // Verify main.py exists at initialization
    QFileInfo mainPy(QDir(m_footFunctionPath).filePath("main.py"));
    if (mainPy.exists()) {
        appendOutput("Verified: main.py found at " + mainPy.absoluteFilePath(), false);
    } else {
        appendOutput("WARNING: main.py not found at " + QDir(m_footFunctionPath).filePath("main.py"), true);
        appendOutput("Please verify the application is in the correct location relative to foot-Function.", true);
    }
    
    // Find Python interpreter with OpenCV
    m_pythonExecutable = findPythonWithOpenCV();
    if (!m_pythonExecutable.isEmpty()) {
        appendOutput("Python interpreter: " + m_pythonExecutable, false);
    } else {
        appendOutput("WARNING: No Python interpreter with OpenCV found. Analysis may fail.", true);
    }
}

MainWindow::~MainWindow()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        if (!m_process->waitForFinished(PROCESS_KILL_TIMEOUT_MS)) {
            m_process->kill();
        }
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Football Analysis - Qt GUI");
    resize(1200, 900);
    
    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Add components
    mainLayout->addWidget(createInputGroup());
    
    // Create horizontal layout for output and results
    QHBoxLayout* middleLayout = new QHBoxLayout();
    middleLayout->addWidget(createOutputGroup(), 1);
    middleLayout->addWidget(createResultsGroup(), 1);
    mainLayout->addLayout(middleLayout);
    
    // Add video player
    mainLayout->addWidget(createVideoPlayerGroup());
    
    mainLayout->addLayout(createControlButtons());
    
    // Status bar
    m_statusLabel = new QLabel("Ready", this);
    statusBar()->addWidget(m_statusLabel);
    
    setCentralWidget(centralWidget);
}

QGroupBox* MainWindow::createInputGroup()
{
    QGroupBox* group = new QGroupBox("Input Configuration", this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    
    // Input Video
    QHBoxLayout* videoLayout = new QHBoxLayout();
    QLabel* videoLabel = new QLabel("Input Video:", this);
    videoLabel->setMinimumWidth(100);
    m_inputVideoEdit = new QLineEdit(this);
    m_browseInputBtn = new QPushButton("Browse...", this);
    videoLayout->addWidget(videoLabel);
    videoLayout->addWidget(m_inputVideoEdit);
    videoLayout->addWidget(m_browseInputBtn);
    layout->addLayout(videoLayout);
    
    // Model File
    QHBoxLayout* modelLayout = new QHBoxLayout();
    QLabel* modelLabel = new QLabel("Model File:", this);
    modelLabel->setMinimumWidth(100);
    m_modelFileEdit = new QLineEdit(this);
    m_browseModelBtn = new QPushButton("Browse...", this);
    modelLayout->addWidget(modelLabel);
    modelLayout->addWidget(m_modelFileEdit);
    modelLayout->addWidget(m_browseModelBtn);
    layout->addLayout(modelLayout);
    
    // Output Directory
    QHBoxLayout* outputLayout = new QHBoxLayout();
    QLabel* outputLabel = new QLabel("Output Directory:", this);
    outputLabel->setMinimumWidth(100);
    m_outputDirEdit = new QLineEdit(this);
    m_browseOutputBtn = new QPushButton("Browse...", this);
    outputLayout->addWidget(outputLabel);
    outputLayout->addWidget(m_outputDirEdit);
    outputLayout->addWidget(m_browseOutputBtn);
    layout->addLayout(outputLayout);
    
    // Use Stubs Checkbox
    QHBoxLayout* stubsLayout = new QHBoxLayout();
    m_useStubsCheckbox = new QCheckBox("Use Cached Stubs (faster processing)", this);
    stubsLayout->addWidget(m_useStubsCheckbox);
    stubsLayout->addStretch();
    layout->addLayout(stubsLayout);
    
    // Connect buttons
    connect(m_browseInputBtn, &QPushButton::clicked, this, &MainWindow::onBrowseInputVideo);
    connect(m_browseModelBtn, &QPushButton::clicked, this, &MainWindow::onBrowseModelFile);
    connect(m_browseOutputBtn, &QPushButton::clicked, this, &MainWindow::onBrowseOutputDir);
    
    return group;
}

QGroupBox* MainWindow::createOutputGroup()
{
    QGroupBox* group = new QGroupBox("Process Output", this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    
    m_outputConsole = new QTextEdit(this);
    m_outputConsole->setReadOnly(true);
    m_outputConsole->setFont(QFont("Courier", 9));
    m_outputConsole->setLineWrapMode(QTextEdit::NoWrap);
    
    layout->addWidget(m_outputConsole);
    
    return group;
}

QGroupBox* MainWindow::createResultsGroup()
{
    QGroupBox* group = new QGroupBox("Analysis Results", this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    
    // Team possession percentages
    QLabel* possessionTitle = new QLabel("<b>Ball Possession</b>", this);
    layout->addWidget(possessionTitle);
    
    m_team1PossessionLabel = new QLabel("Team 1: --", this);
    m_team1PossessionLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    layout->addWidget(m_team1PossessionLabel);
    
    m_team2PossessionLabel = new QLabel("Team 2: --", this);
    m_team2PossessionLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    layout->addWidget(m_team2PossessionLabel);
    
    layout->addSpacing(20);
    
    // Distance covered
    QLabel* distanceTitle = new QLabel("<b>Total Distance Covered</b>", this);
    layout->addWidget(distanceTitle);
    
    m_team1DistanceLabel = new QLabel("Team 1: --", this);
    m_team1DistanceLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    layout->addWidget(m_team1DistanceLabel);
    
    m_team2DistanceLabel = new QLabel("Team 2: --", this);
    m_team2DistanceLabel->setStyleSheet("font-size: 14px; padding: 5px;");
    layout->addWidget(m_team2DistanceLabel);
    
    layout->addStretch();
    
    return group;
}

QGroupBox* MainWindow::createVideoPlayerGroup()
{
    QGroupBox* group = new QGroupBox("Output Video Player", this);
    QVBoxLayout* layout = new QVBoxLayout(group);
    
    // Video widget
    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setMinimumHeight(300);
    layout->addWidget(m_videoWidget);
    
    // Media player setup
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    
    // Playback controls
    QHBoxLayout* controlLayout = new QHBoxLayout();
    
    m_playBtn = new QPushButton("Play", this);
    m_pauseBtn = new QPushButton("Pause", this);
    m_stopVideoBtn = new QPushButton("Stop", this);
    
    m_playBtn->setEnabled(false);
    m_pauseBtn->setEnabled(false);
    m_stopVideoBtn->setEnabled(false);
    
    controlLayout->addWidget(m_playBtn);
    controlLayout->addWidget(m_pauseBtn);
    controlLayout->addWidget(m_stopVideoBtn);
    controlLayout->addStretch();
    
    layout->addLayout(controlLayout);
    
    // Connect signals
    connect(m_playBtn, &QPushButton::clicked, this, &MainWindow::onPlayVideo);
    connect(m_pauseBtn, &QPushButton::clicked, this, &MainWindow::onPauseVideo);
    connect(m_stopVideoBtn, &QPushButton::clicked, this, &MainWindow::onStopVideo);
    
    return group;
}

QHBoxLayout* MainWindow::createControlButtons()
{
    QHBoxLayout* layout = new QHBoxLayout();
    
    m_runBtn = new QPushButton("Run Analysis", this);
    m_runBtn->setMinimumHeight(40);
    m_runBtn->setStyleSheet("QPushButton { font-weight: bold; font-size: 14px; }");
    
    m_stopBtn = new QPushButton("Stop", this);
    m_stopBtn->setMinimumHeight(40);
    m_stopBtn->setEnabled(false);
    
    layout->addStretch();
    layout->addWidget(m_runBtn);
    layout->addWidget(m_stopBtn);
    layout->addStretch();
    
    connect(m_runBtn, &QPushButton::clicked, this, &MainWindow::onRunAnalysis);
    connect(m_stopBtn, &QPushButton::clicked, this, &MainWindow::onStopAnalysis);
    
    return layout;
}

void MainWindow::onBrowseInputVideo()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select Input Video",
        m_inputVideoEdit->text().isEmpty() ? QDir(m_footFunctionPath).filePath("input_videos") : m_inputVideoEdit->text(),
        "Video Files (*.mp4 *.avi *.mov *.mkv);;All Files (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        m_inputVideoEdit->setText(fileName);
    }
}

void MainWindow::onBrowseModelFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select Model File",
        m_modelFileEdit->text().isEmpty() ? QDir(m_footFunctionPath).filePath("models") : m_modelFileEdit->text(),
        "Model Files (*.pt *.pth);;All Files (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        m_modelFileEdit->setText(fileName);
    }
}

void MainWindow::onBrowseOutputDir()
{
    QString dirName = QFileDialog::getExistingDirectory(
        this,
        "Select Output Directory",
        m_outputDirEdit->text().isEmpty() ? QDir(m_footFunctionPath).filePath("output_videos") : m_outputDirEdit->text()
    );
    
    if (!dirName.isEmpty()) {
        m_outputDirEdit->setText(dirName);
    }
}

void MainWindow::onRunAnalysis()
{
    if (!validateInputs()) {
        return;
    }
    
    // Clear output console
    m_outputConsole->clear();
    
    appendOutput(QString("=").repeated(SEPARATOR_LENGTH), false);
    appendOutput("Starting Football Analysis...", false);
    appendOutput("Time: " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), false);
    appendOutput(QString("=").repeated(SEPARATOR_LENGTH), false);
    
    // Use the Python executable found at initialization
    QString pythonExecutable = m_pythonExecutable.isEmpty() ? "python3" : m_pythonExecutable;
    QString scriptPath = QDir(m_footFunctionPath).filePath("main.py");
    
    // Build arguments by modifying the script's configuration
    // Since we can't modify the script, we'll pass environment variables
    // or create a temporary wrapper script that imports and runs the pipeline
    
    // For this implementation, we'll create a wrapper Python script
    QString wrapperScript = QString(
        "#!/usr/bin/env python3\n"
        "import sys\n"
        "sys.path.insert(0, '%1')\n"
        "from main import VideoAnalysisPipeline\n"
        "try:\n"
        "    pipeline = VideoAnalysisPipeline(\n"
        "        input_video_path='%2',\n"
        "        model_path='%3',\n"
        "        output_dir='%4',\n"
        "        use_stubs=%5\n"
        "    )\n"
        "    pipeline.run()\n"
        "    print('\\n=== Analysis completed successfully ===')\n"
        "    sys.exit(0)\n"
        "except Exception as e:\n"
        "    print(f'\\nERROR: {e}', file=sys.stderr)\n"
        "    sys.exit(1)\n"
    ).arg(m_footFunctionPath)
     .arg(m_inputVideoEdit->text())
     .arg(m_modelFileEdit->text())
     .arg(m_outputDirEdit->text())
     .arg(m_useStubsCheckbox->isChecked() ? "True" : "False");
    
    // Write wrapper script to temporary file with unique name
    QTemporaryFile* tempFile = new QTemporaryFile(
        QDir(QDir::tempPath()).filePath("foot_analysis_wrapper_XXXXXX.py"),
        this
    );
    tempFile->setAutoRemove(true);  // Auto-cleanup when process finishes
    
    if (!tempFile->open()) {
        QMessageBox::critical(this, "Error", "Failed to create temporary script file.");
        delete tempFile;
        return;
    }
    
    QTextStream out(tempFile);
    out << wrapperScript;
    out.flush();
    
    QString tempScriptPath = tempFile->fileName();
    tempFile->close();
    // Note: Keep tempFile object alive until process finishes
    // (it's owned by 'this', so it will be cleaned up when MainWindow is destroyed)
    
    appendOutput("Input Video: " + m_inputVideoEdit->text(), false);
    appendOutput("Model File: " + m_modelFileEdit->text(), false);
    appendOutput("Output Directory: " + m_outputDirEdit->text(), false);
    appendOutput("Use Stubs: " + QString(m_useStubsCheckbox->isChecked() ? "Yes" : "No"), false);
    appendOutput("", false);
    
    // Set working directory to foot-Function
    m_process->setWorkingDirectory(m_footFunctionPath);
    
    // Start process
    QStringList arguments;
    arguments << tempScriptPath;
    
    appendOutput("Command: " + pythonExecutable + " " + arguments.join(" "), false);
    appendOutput("Working Directory: " + m_footFunctionPath, false);
    appendOutput(QString("=").repeated(SEPARATOR_LENGTH), false);
    appendOutput("", false);
    
    m_process->start(pythonExecutable, arguments);
    
    if (!m_process->waitForStarted(PROCESS_START_TIMEOUT_MS)) {
        appendOutput("ERROR: Failed to start process!", true);
        QMessageBox::critical(this, "Error", "Failed to start Python process.\n\nMake sure Python 3 is installed and accessible.");
    }
}

void MainWindow::onStopAnalysis()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        appendOutput("", false);
        appendOutput(QString("=").repeated(SEPARATOR_LENGTH), false);
        appendOutput("Stopping process...", false);
        
        m_process->terminate();
        
        if (!m_process->waitForFinished(PROCESS_TERMINATE_TIMEOUT_MS)) {
            appendOutput("Process did not terminate gracefully, killing...", false);
            m_process->kill();
        }
    }
}

void MainWindow::onProcessReadyReadStdout()
{
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());
    appendOutput(output, false);
}

void MainWindow::onProcessReadyReadStderr()
{
    QString output = QString::fromUtf8(m_process->readAllStandardError());
    appendOutput(output, true);
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    appendOutput("", false);
    appendOutput(QString("=").repeated(SEPARATOR_LENGTH), false);
    
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            appendOutput("Process completed successfully!", false);
            m_statusLabel->setText("Analysis completed successfully");
            
            // Parse and display results
            QString outputDir = m_outputDirEdit->text();
            m_outputDataPath = QDir(outputDir).filePath("data_output.json");
            m_outputVideoPath = QDir(outputDir).filePath("output_video.avi");
            
            // Load results if JSON file exists
            if (QFileInfo::exists(m_outputDataPath)) {
                parseAndDisplayResults(m_outputDataPath);
            }
            
            // Load video if it exists
            if (QFileInfo::exists(m_outputVideoPath)) {
                loadOutputVideo(m_outputVideoPath);
            }
            
            QMessageBox::information(this, "Success", 
                "Football analysis completed successfully!\n\n"
                "Output files saved to:\n" + outputDir);
        } else {
            appendOutput(QString("Process exited with code: %1").arg(exitCode), true);
            m_statusLabel->setText(QString("Process failed with exit code %1").arg(exitCode));
        }
    } else {
        appendOutput("Process crashed or was terminated!", true);
        m_statusLabel->setText("Process crashed or terminated");
    }
    
    appendOutput(QString("=").repeated(SEPARATOR_LENGTH), false);
    
    setControlsEnabled(true);
}

void MainWindow::onProcessError(QProcess::ProcessError error)
{
    QString errorString;
    
    switch (error) {
        case QProcess::FailedToStart:
            errorString = "Failed to start process. Check if Python 3 is installed.";
            break;
        case QProcess::Crashed:
            errorString = "Process crashed.";
            break;
        case QProcess::Timedout:
            errorString = "Process timed out.";
            break;
        case QProcess::WriteError:
            errorString = "Write error.";
            break;
        case QProcess::ReadError:
            errorString = "Read error.";
            break;
        case QProcess::UnknownError:
        default:
            errorString = "Unknown error occurred.";
            break;
    }
    
    appendOutput("", false);
    appendOutput("ERROR: " + errorString, true);
    m_statusLabel->setText("Error: " + errorString);
}

void MainWindow::onProcessStateChanged(QProcess::ProcessState state)
{
    switch (state) {
        case QProcess::NotRunning:
            m_statusLabel->setText("Ready");
            setControlsEnabled(true);
            break;
        case QProcess::Starting:
            m_statusLabel->setText("Starting process...");
            setControlsEnabled(false);
            break;
        case QProcess::Running:
            m_statusLabel->setText("Running analysis...");
            setControlsEnabled(false);
            break;
    }
}

bool MainWindow::validateInputs()
{
    // Check input video
    if (m_inputVideoEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please specify an input video file.");
        return false;
    }
    
    QFileInfo inputVideo(m_inputVideoEdit->text());
    if (!inputVideo.exists() || !inputVideo.isFile()) {
        QMessageBox::warning(this, "Validation Error", 
            "Input video file does not exist:\n" + m_inputVideoEdit->text());
        return false;
    }
    
    // Check model file
    if (m_modelFileEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please specify a model file.");
        return false;
    }
    
    QFileInfo modelFile(m_modelFileEdit->text());
    if (!modelFile.exists() || !modelFile.isFile()) {
        QMessageBox::warning(this, "Validation Error", 
            "Model file does not exist:\n" + m_modelFileEdit->text());
        return false;
    }
    
    // Check output directory
    if (m_outputDirEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please specify an output directory.");
        return false;
    }
    
    // Create output directory if it doesn't exist
    QDir outputDir(m_outputDirEdit->text());
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            QMessageBox::warning(this, "Validation Error", 
                "Failed to create output directory:\n" + m_outputDirEdit->text());
            return false;
        }
    }
    
    // Check if foot-Function exists
    QDir footFunctionDir(m_footFunctionPath);
    if (!footFunctionDir.exists()) {
        QMessageBox::warning(this, "Validation Error", 
            "foot-Function directory not found:\n" + m_footFunctionPath + 
            "\n\nPlease ensure the application is in the correct location.");
        return false;
    }
    
    // Check if main.py exists
    QFileInfo mainScript(QDir(m_footFunctionPath).filePath("main.py"));
    if (!mainScript.exists()) {
        QMessageBox::warning(this, "Validation Error", 
            "main.py not found in foot-Function directory:\n" + m_footFunctionPath);
        return false;
    }
    
    return true;
}

void MainWindow::appendOutput(const QString& text, bool isError)
{
    if (text.isEmpty()) {
        m_outputConsole->append("");
        return;
    }
    
    // Set text color
    QTextCharFormat format;
    if (isError) {
        format.setForeground(QBrush(QColor(255, 0, 0))); // Red for errors
    } else {
        format.setForeground(QBrush(QColor(0, 0, 0))); // Black for normal output
    }
    
    // Append text
    QTextCursor cursor = m_outputConsole->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text + "\n", format);
    
    // Auto-scroll to bottom
    m_outputConsole->verticalScrollBar()->setValue(
        m_outputConsole->verticalScrollBar()->maximum()
    );
}

void MainWindow::setControlsEnabled(bool enabled)
{
    m_inputVideoEdit->setEnabled(enabled);
    m_modelFileEdit->setEnabled(enabled);
    m_outputDirEdit->setEnabled(enabled);
    m_useStubsCheckbox->setEnabled(enabled);
    m_browseInputBtn->setEnabled(enabled);
    m_browseModelBtn->setEnabled(enabled);
    m_browseOutputBtn->setEnabled(enabled);
    m_runBtn->setEnabled(enabled);
    m_stopBtn->setEnabled(!enabled);
}

QString MainWindow::findPythonWithOpenCV()
{
    // List of potential Python executables to try
    QStringList pythonCandidates;
    
    // Add common Python paths
    pythonCandidates << "/usr/bin/python3"
                     << "/usr/local/bin/python3"
                     << "/usr/bin/python"
                     << "/usr/local/bin/python"
                     << "python3"
                     << "python";
    
    // On Windows, also check common installation paths
#ifdef Q_OS_WIN
    pythonCandidates << "C:/Python312/python.exe"
                     << "C:/Python311/python.exe"
                     << "C:/Python310/python.exe"
                     << "C:/Python39/python.exe"
                     << "C:/Python38/python.exe"
                     << "C:/Program Files/Python312/python.exe"
                     << "C:/Program Files/Python311/python.exe"
                     << "C:/Program Files/Python310/python.exe";
#endif
    
    // Try each candidate
    for (const QString& candidate : pythonCandidates) {
        QProcess testProcess;
        testProcess.start(candidate, QStringList() << "-c" << "import cv2; print('OK')");
        
        if (testProcess.waitForStarted(2000)) {
            if (testProcess.waitForFinished(5000)) {
                QString output = QString::fromUtf8(testProcess.readAllStandardOutput()).trimmed();
                if (output == "OK" && testProcess.exitCode() == 0) {
                    // Found a Python with OpenCV
                    return candidate;
                }
            } else {
                testProcess.kill();
            }
        }
    }
    
    // If no Python with OpenCV found, return python3 as fallback
    return "python3";
}

void MainWindow::parseAndDisplayResults(const QString& jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        appendOutput("ERROR: Could not open results file: " + jsonPath, true);
        return;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
        appendOutput("ERROR: Invalid JSON format in results file", true);
        return;
    }
    
    QJsonObject root = doc.object();
    
    // Parse summary data
    if (root.contains("summary")) {
        QJsonObject summary = root["summary"].toObject();
        
        // Team 1 possession
        if (summary.contains("team_1_possession_percent")) {
            double team1Poss = summary["team_1_possession_percent"].toDouble();
            m_team1PossessionLabel->setText(QString("Team 1: %1%").arg(team1Poss, 0, 'f', 2));
        }
        
        // Team 2 possession
        if (summary.contains("team_2_possession_percent")) {
            double team2Poss = summary["team_2_possession_percent"].toDouble();
            m_team2PossessionLabel->setText(QString("Team 2: %1%").arg(team2Poss, 0, 'f', 2));
        }
    }
    
    // Calculate total distance for each team
    double team1TotalKm = 0.0;
    double team2TotalKm = 0.0;
    int team1Players = 0;
    int team2Players = 0;
    
    if (root.contains("team_1")) {
        QJsonObject team1 = root["team_1"].toObject();
        for (const QString& playerId : team1.keys()) {
            QJsonObject playerData = team1[playerId].toObject();
            if (playerData.contains("distance_km")) {
                team1TotalKm += playerData["distance_km"].toDouble();
                team1Players++;
            }
        }
    }
    
    if (root.contains("team_2")) {
        QJsonObject team2 = root["team_2"].toObject();
        for (const QString& playerId : team2.keys()) {
            QJsonObject playerData = team2[playerId].toObject();
            if (playerData.contains("distance_km")) {
                team2TotalKm += playerData["distance_km"].toDouble();
                team2Players++;
            }
        }
    }
    
    // Display team distances
    m_team1DistanceLabel->setText(QString("Team 1: %1 km (%2 players)")
                                   .arg(team1TotalKm, 0, 'f', 2)
                                   .arg(team1Players));
    m_team2DistanceLabel->setText(QString("Team 2: %1 km (%2 players)")
                                   .arg(team2TotalKm, 0, 'f', 2)
                                   .arg(team2Players));
    
    appendOutput("Results displayed successfully", false);
}

void MainWindow::loadOutputVideo(const QString& videoPath)
{
    if (!QFileInfo::exists(videoPath)) {
        appendOutput("ERROR: Video file not found: " + videoPath, true);
        return;
    }
    
    // Load video into media player
    m_mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));
    
    // Enable playback controls
    m_playBtn->setEnabled(true);
    m_pauseBtn->setEnabled(true);
    m_stopVideoBtn->setEnabled(true);
    
    appendOutput("Output video loaded: " + videoPath, false);
    appendOutput("Use Play/Pause/Stop buttons to control playback", false);
}

void MainWindow::onPlayVideo()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->play();
    }
}

void MainWindow::onPauseVideo()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->pause();
    }
}

void MainWindow::onStopVideo()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }
}
