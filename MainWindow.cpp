#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QScrollBar>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QPixmap>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_process(nullptr)
{
    setupUI();
    
    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &MainWindow::onProcessReadyReadStdout);
    connect(m_process, &QProcess::readyReadStandardError, this, &MainWindow::onProcessReadyReadStderr);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &MainWindow::onProcessError);
    
    appendOutput("Football Analysis GUI initialized.", false);
    appendOutput("Configure paths and click Run Analysis to start.", false);
}

MainWindow::~MainWindow()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(3000);
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Football Analysis - Qt GUI");
    resize(1000, 800);
    
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Configuration Group
    QGroupBox* configGroup = new QGroupBox("Configuration", this);
    QFormLayout* configLayout = new QFormLayout(configGroup);
    
    // Python path
    QHBoxLayout* pythonLayout = new QHBoxLayout();
    m_pythonPathEdit = new QLineEdit(this);
    m_pythonPathEdit->setPlaceholderText("Path to Python executable (e.g., C:/Python39/python.exe)");
    m_browsePythonBtn = new QPushButton("Browse", this);
    pythonLayout->addWidget(m_pythonPathEdit);
    pythonLayout->addWidget(m_browsePythonBtn);
    configLayout->addRow("Python:", pythonLayout);
    connect(m_browsePythonBtn, &QPushButton::clicked, this, &MainWindow::onBrowsePython);
    
    // Script path
    QHBoxLayout* scriptLayout = new QHBoxLayout();
    m_scriptPathEdit = new QLineEdit(this);
    m_scriptPathEdit->setPlaceholderText("Path to Python script (e.g., foot-Function/main.py)");
    m_browseScriptBtn = new QPushButton("Browse", this);
    scriptLayout->addWidget(m_scriptPathEdit);
    scriptLayout->addWidget(m_browseScriptBtn);
    configLayout->addRow("Script:", scriptLayout);
    connect(m_browseScriptBtn, &QPushButton::clicked, this, &MainWindow::onBrowseScript);
    
    // Video path
    QHBoxLayout* videoLayout = new QHBoxLayout();
    m_videoPathEdit = new QLineEdit(this);
    m_videoPathEdit->setPlaceholderText("Input video file");
    m_browseVideoBtn = new QPushButton("Browse", this);
    videoLayout->addWidget(m_videoPathEdit);
    videoLayout->addWidget(m_browseVideoBtn);
    configLayout->addRow("Video:", videoLayout);
    connect(m_browseVideoBtn, &QPushButton::clicked, this, &MainWindow::onBrowseVideo);
    
    // Model path
    QHBoxLayout* modelLayout = new QHBoxLayout();
    m_modelPathEdit = new QLineEdit(this);
    m_modelPathEdit->setPlaceholderText("Model file (.pt)");
    m_browseModelBtn = new QPushButton("Browse", this);
    modelLayout->addWidget(m_modelPathEdit);
    modelLayout->addWidget(m_browseModelBtn);
    configLayout->addRow("Model:", modelLayout);
    connect(m_browseModelBtn, &QPushButton::clicked, this, &MainWindow::onBrowseModel);
    
    // Output directory
    QHBoxLayout* outputLayout = new QHBoxLayout();
    m_outputDirEdit = new QLineEdit(this);
    m_outputDirEdit->setPlaceholderText("Output directory");
    m_browseOutputBtn = new QPushButton("Browse", this);
    outputLayout->addWidget(m_outputDirEdit);
    outputLayout->addWidget(m_browseOutputBtn);
    configLayout->addRow("Output:", outputLayout);
    connect(m_browseOutputBtn, &QPushButton::clicked, this, &MainWindow::onBrowseOutput);
    
    mainLayout->addWidget(configGroup);
    
    // Middle section: Output console and Results
    QHBoxLayout* middleLayout = new QHBoxLayout();
    
    // Output console
    QGroupBox* outputGroup = new QGroupBox("Process Output", this);
    QVBoxLayout* outputVLayout = new QVBoxLayout(outputGroup);
    m_outputConsole = new QTextEdit(this);
    m_outputConsole->setReadOnly(true);
    m_outputConsole->setFont(QFont("Courier", 9));
    outputVLayout->addWidget(m_outputConsole);
    middleLayout->addWidget(outputGroup, 2);
    
    // Results panel
    QGroupBox* resultsGroup = new QGroupBox("Analysis Results", this);
    QVBoxLayout* resultsLayout = new QVBoxLayout(resultsGroup);
    
    QLabel* possessionTitle = new QLabel("<b>Ball Possession</b>", this);
    resultsLayout->addWidget(possessionTitle);
    m_team1PossessionLabel = new QLabel("Team 1: --", this);
    m_team2PossessionLabel = new QLabel("Team 2: --", this);
    resultsLayout->addWidget(m_team1PossessionLabel);
    resultsLayout->addWidget(m_team2PossessionLabel);
    
    resultsLayout->addSpacing(10);
    
    QLabel* distanceTitle = new QLabel("<b>Distance Covered</b>", this);
    resultsLayout->addWidget(distanceTitle);
    m_team1DistanceLabel = new QLabel("Team 1: --", this);
    m_team2DistanceLabel = new QLabel("Team 2: --", this);
    resultsLayout->addWidget(m_team1DistanceLabel);
    resultsLayout->addWidget(m_team2DistanceLabel);
    
    resultsLayout->addStretch();
    middleLayout->addWidget(resultsGroup, 1);
    
    mainLayout->addLayout(middleLayout);
    
    // Video placeholder
    QGroupBox* videoGroup = new QGroupBox("Output Video", this);
    QVBoxLayout* videoLayout = new QVBoxLayout(videoGroup);
    m_videoPlaceholder = new QLabel("Video will appear here after analysis completes", this);
    m_videoPlaceholder->setAlignment(Qt::AlignCenter);
    m_videoPlaceholder->setMinimumHeight(200);
    m_videoPlaceholder->setStyleSheet("QLabel { background-color: #2b2b2b; color: #888; font-size: 14px; }");
    videoLayout->addWidget(m_videoPlaceholder);
    mainLayout->addWidget(videoGroup);
    
    // Control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_runBtn = new QPushButton("Run Analysis", this);
    m_runBtn->setMinimumHeight(40);
    m_stopBtn = new QPushButton("Stop", this);
    m_stopBtn->setMinimumHeight(40);
    m_stopBtn->setEnabled(false);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_runBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    connect(m_runBtn, &QPushButton::clicked, this, &MainWindow::onRunAnalysis);
    connect(m_stopBtn, &QPushButton::clicked, this, &MainWindow::onStopAnalysis);
    
    // Status bar
    m_statusLabel = new QLabel("Ready", this);
    statusBar()->addWidget(m_statusLabel);
    
    setCentralWidget(centralWidget);
}

void MainWindow::onBrowsePython()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Python Executable", 
        m_pythonPathEdit->text().isEmpty() ? "" : m_pythonPathEdit->text(),
        "Executable Files (*.exe);;All Files (*)");
    if (!fileName.isEmpty()) {
        m_pythonPathEdit->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onBrowseScript()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Python Script",
        m_scriptPathEdit->text().isEmpty() ? "" : m_scriptPathEdit->text(),
        "Python Files (*.py);;All Files (*)");
    if (!fileName.isEmpty()) {
        m_scriptPathEdit->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onBrowseVideo()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Video File",
        m_videoPathEdit->text().isEmpty() ? "" : m_videoPathEdit->text(),
        "Video Files (*.mp4 *.avi *.mov);;All Files (*)");
    if (!fileName.isEmpty()) {
        m_videoPathEdit->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onBrowseModel()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Model File",
        m_modelPathEdit->text().isEmpty() ? "" : m_modelPathEdit->text(),
        "Model Files (*.pt *.pth);;All Files (*)");
    if (!fileName.isEmpty()) {
        m_modelPathEdit->setText(QDir::toNativeSeparators(fileName));
    }
}

void MainWindow::onBrowseOutput()
{
    QString dirName = QFileDialog::getExistingDirectory(this, "Select Output Directory",
        m_outputDirEdit->text().isEmpty() ? "" : m_outputDirEdit->text());
    if (!dirName.isEmpty()) {
        m_outputDirEdit->setText(QDir::toNativeSeparators(dirName));
    }
}

bool MainWindow::validateInputs()
{
    if (m_pythonPathEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please specify Python executable path.");
        return false;
    }
    
    if (!QFileInfo::exists(m_pythonPathEdit->text())) {
        QMessageBox::warning(this, "Validation Error", "Python executable not found.");
        return false;
    }
    
    if (m_scriptPathEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please specify Python script path.");
        return false;
    }
    
    if (!QFileInfo::exists(m_scriptPathEdit->text())) {
        QMessageBox::warning(this, "Validation Error", "Python script not found.");
        return false;
    }
    
    if (m_videoPathEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please specify input video file.");
        return false;
    }
    
    if (!QFileInfo::exists(m_videoPathEdit->text())) {
        QMessageBox::warning(this, "Validation Error", "Input video file not found.");
        return false;
    }
    
    if (m_modelPathEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please specify model file.");
        return false;
    }
    
    if (!QFileInfo::exists(m_modelPathEdit->text())) {
        QMessageBox::warning(this, "Validation Error", "Model file not found.");
        return false;
    }
    
    if (m_outputDirEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please specify output directory.");
        return false;
    }
    
    return true;
}

void MainWindow::onRunAnalysis()
{
    if (!validateInputs()) {
        return;
    }
    
    m_outputConsole->clear();
    
    appendOutput("==========================================================", false);
    appendOutput("Starting Football Analysis...", false);
    appendOutput("==========================================================", false);
    
    // Create wrapper script
    QString scriptDir = QFileInfo(m_scriptPathEdit->text()).absolutePath();
    QString wrapperScript = QString(
        "import sys\n"
        "sys.path.insert(0, r'%1')\n"
        "from main import VideoAnalysisPipeline\n"
        "try:\n"
        "    pipeline = VideoAnalysisPipeline(\n"
        "        input_video_path=r'%2',\n"
        "        model_path=r'%3',\n"
        "        output_dir=r'%4',\n"
        "        use_stubs=True\n"
        "    )\n"
        "    pipeline.run()\n"
        "    print('\\nAnalysis completed successfully!')\n"
        "except Exception as e:\n"
        "    print(f'\\nError: {e}', file=sys.stderr)\n"
        "    sys.exit(1)\n"
    ).arg(scriptDir)
     .arg(m_videoPathEdit->text())
     .arg(m_modelPathEdit->text())
     .arg(m_outputDirEdit->text());
    
    QString tempScriptPath = QDir::temp().filePath("foot_analysis_wrapper.py");
    QFile tempFile(tempScriptPath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        appendOutput("ERROR: Failed to create temporary script file!", true);
        return;
    }
    
    QTextStream out(&tempFile);
    out << wrapperScript;
    tempFile.close();
    
    appendOutput("Python: " + m_pythonPathEdit->text(), false);
    appendOutput("Script: " + m_scriptPathEdit->text(), false);
    appendOutput("Video: " + m_videoPathEdit->text(), false);
    appendOutput("Model: " + m_modelPathEdit->text(), false);
    appendOutput("Output: " + m_outputDirEdit->text(), false);
    appendOutput("", false);
    
    m_process->setWorkingDirectory(scriptDir);
    
    QStringList arguments;
    arguments << tempScriptPath;
    
    appendOutput("Starting Python process...", false);
    appendOutput("", false);
    
    m_process->start(m_pythonPathEdit->text(), arguments);
    
    if (!m_process->waitForStarted(5000)) {
        appendOutput("ERROR: Failed to start Python process!", true);
        setControlsEnabled(true);
        return;
    }
    
    setControlsEnabled(false);
    m_statusLabel->setText("Running analysis...");
}

void MainWindow::onStopAnalysis()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        appendOutput("", false);
        appendOutput("Stopping process...", false);
        m_process->terminate();
        if (!m_process->waitForFinished(5000)) {
            m_process->kill();
        }
        appendOutput("Process stopped.", false);
        setControlsEnabled(true);
        m_statusLabel->setText("Stopped");
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
    appendOutput("==========================================================", false);
    
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        appendOutput("Process completed successfully!", false);
        m_statusLabel->setText("Analysis completed");
        
        // Parse results
        parseResults(m_outputDirEdit->text());
        
        QMessageBox::information(this, "Success", 
            "Football analysis completed successfully!\n\n"
            "Output files saved to:\n" + m_outputDirEdit->text());
    } else {
        appendOutput(QString("Process exited with code: %1").arg(exitCode), true);
        m_statusLabel->setText(QString("Process failed (exit code %1)").arg(exitCode));
    }
    
    appendOutput("==========================================================", false);
    setControlsEnabled(true);
}

void MainWindow::onProcessError(QProcess::ProcessError error)
{
    QString errorString;
    switch (error) {
        case QProcess::FailedToStart:
            errorString = "Failed to start process. Check Python path.";
            break;
        case QProcess::Crashed:
            errorString = "Process crashed.";
            break;
        case QProcess::Timedout:
            errorString = "Process timed out.";
            break;
        case QProcess::ReadError:
            errorString = "Read error occurred.";
            break;
        case QProcess::WriteError:
            errorString = "Write error occurred.";
            break;
        default:
            errorString = "Unknown error occurred.";
    }
    
    appendOutput("ERROR: " + errorString, true);
    m_statusLabel->setText("Error: " + errorString);
}

void MainWindow::appendOutput(const QString& text, bool isError)
{
    if (text.isEmpty()) {
        m_outputConsole->append("");
        return;
    }
    
    QTextCharFormat format;
    if (isError) {
        format.setForeground(QBrush(QColor(255, 100, 100)));
    } else {
        format.setForeground(QBrush(QColor(200, 200, 200)));
    }
    
    QTextCursor cursor = m_outputConsole->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text + "\n", format);
    
    m_outputConsole->verticalScrollBar()->setValue(
        m_outputConsole->verticalScrollBar()->maximum()
    );
}

void MainWindow::setControlsEnabled(bool enabled)
{
    m_pythonPathEdit->setEnabled(enabled);
    m_scriptPathEdit->setEnabled(enabled);
    m_videoPathEdit->setEnabled(enabled);
    m_modelPathEdit->setEnabled(enabled);
    m_outputDirEdit->setEnabled(enabled);
    m_browsePythonBtn->setEnabled(enabled);
    m_browseScriptBtn->setEnabled(enabled);
    m_browseVideoBtn->setEnabled(enabled);
    m_browseModelBtn->setEnabled(enabled);
    m_browseOutputBtn->setEnabled(enabled);
    m_runBtn->setEnabled(enabled);
    m_stopBtn->setEnabled(!enabled);
}

void MainWindow::parseResults(const QString& outputDir)
{
    QString jsonPath = QDir(outputDir).filePath("data_output.json");
    
    if (!QFileInfo::exists(jsonPath)) {
        appendOutput("Results file not found: " + jsonPath, true);
        return;
    }
    
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        appendOutput("Could not open results file", true);
        return;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
        appendOutput("Invalid JSON format", true);
        return;
    }
    
    QJsonObject root = doc.object();
    
    // Parse team possession
    if (root.contains("summary")) {
        QJsonObject summary = root["summary"].toObject();
        
        if (summary.contains("team_1_possession_percent")) {
            double team1Poss = summary["team_1_possession_percent"].toDouble();
            m_team1PossessionLabel->setText(QString("Team 1: %1%").arg(team1Poss, 0, 'f', 2));
        }
        
        if (summary.contains("team_2_possession_percent")) {
            double team2Poss = summary["team_2_possession_percent"].toDouble();
            m_team2PossessionLabel->setText(QString("Team 2: %1%").arg(team2Poss, 0, 'f', 2));
        }
    }
    
    // Calculate total distance per team
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
    
    m_team1DistanceLabel->setText(QString("Team 1: %1 km (%2 players)")
                                   .arg(team1TotalKm, 0, 'f', 2)
                                   .arg(team1Players));
    m_team2DistanceLabel->setText(QString("Team 2: %1 km (%2 players)")
                                   .arg(team2TotalKm, 0, 'f', 2)
                                   .arg(team2Players));
    
    appendOutput("Results parsed and displayed successfully", false);
    
    // Try to display first frame of output video as placeholder
    QString videoPath = QDir(outputDir).filePath("output_video.avi");
    if (QFileInfo::exists(videoPath)) {
        m_videoPlaceholder->setText("Output video generated:\n" + videoPath + 
                                    "\n\n(Qt Multimedia not available - open file externally to view)");
        appendOutput("Output video: " + videoPath, false);
    }
}
