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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_process(nullptr)
{
    // Determine foot-Function path (prefer application dir, fallback to current)
    QString appDir = QCoreApplication::applicationDirPath();
    QDir footDir(appDir);
    if (footDir.exists("foot-Function")) {
        m_footFunctionPath = footDir.filePath("foot-Function");
    } else {
        // Fallback to current working directory
        m_footFunctionPath = QDir(QDir::currentPath()).filePath("foot-Function");
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
    resize(900, 700);
    
    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Add components
    mainLayout->addWidget(createInputGroup());
    mainLayout->addWidget(createOutputGroup());
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
    
    // Build command
    QString pythonExecutable = "python3";
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
            QMessageBox::information(this, "Success", 
                "Football analysis completed successfully!\n\n"
                "Output files saved to:\n" + m_outputDirEdit->text());
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
