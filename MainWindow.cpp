/*******************************************************************************
 * 主窗口实现
 * 
 * 本文件实现足球分析GUI主窗口，提供以下功能：
 * - 视频分析配置的用户界面
 * - 异步Python进程执行和监控
 * - 实时进度更新和日志显示
 * - 自动结果加载（CSV/JSON数据、标注视频）
 * - 嵌入式视频播放器和播放控制
 * 
 * 执行流程：
 * 1. 用户通过文件浏览器选择输入视频和YOLO模型
 * 2. 用户点击"开始分析"按钮
 * 3. QProcess启动Python脚本（main.py）并传入参数
 * 4. GUI实时捕获stdout/stderr，保持响应
 * 5. 完成后，自动加载并显示结果
 * 6. 用户在表格中查看数据并播放标注视频
 ******************************************************************************/

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

/******************************************************************************
 * 构造函数
 * 
 * 初始化主窗口和所有成员变量。
 * 设置UI、加载样式表并配置窗口属性。
 * 
 * 所有指针初始化为nullptr以确保安全析构。
 ******************************************************************************/
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
    // 加载并应用现代QSS样式表以获得专业外观
    loadStyleSheet();
    
    // 构建整个UI（部件、布局、连接）
    setupUI();
    setWindowTitle("Foot Analysis GUI");
    
    // 配置窗口大小以获得最佳用户体验
    setMinimumSize(900, 700);  // 最小尺寸：900x700以确保可用性
    resize(1200, 900);         // 默认尺寸：1200x900以舒适查看
}

/******************************************************************************
 * 析构函数
 * 
 * 清理资源并确保Python进程终止。
 * 停止视频播放并终止任何正在运行的分析进程。
 ******************************************************************************/
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

/******************************************************************************
 * 实用方法：获取项目根路径
 * 
 * 通过从可执行文件位置向上搜索来定位项目根目录。
 * 项目根目录通过同时存在FootAnalysisGUI.pro和foot-Function目录来识别。
 * 
 * 这使应用程序能够正确工作，无论可执行文件位于何处
 * （构建目录、安装位置等）。
 * 
 * 返回：项目根目录的绝对路径
 ******************************************************************************/
QString MainWindow::getProjectRootPath() const
{
    // 获取包含可执行文件的目录
    QString exeDir = QCoreApplication::applicationDirPath();
    
    // 向上搜索项目根目录（FootAnalysisGUI.pro和foot-Function所在位置）
    QDir dir(exeDir);
    int maxLevelsUp = 5;  // 向上搜索的最大层级数
    
    for (int i = 0; i < maxLevelsUp; ++i) {
        // 检查foot-Function目录是否存在于此处
        if (dir.exists("foot-Function") && dir.exists("FootAnalysisGUI.pro")) {
            qDebug() << "Found project root at:" << dir.absolutePath();
            return dir.absolutePath();
        }
        
        // 向上移动一级
        if (!dir.cdUp()) {
            break;  // 已到达文件系统根目录
        }
    }
    
    // 备用方案：假设foot-Function与可执行文件在同一目录
    // 这处理可执行文件从项目根目录运行的情况
    qDebug() << "Could not find project root, using exe directory:" << exeDir;
    return exeDir;
}

/******************************************************************************
 * UI设置方法：构建用户界面
 * 
 * 使用Qt布局和部件构建完整的UI层次结构。
 * 布局使用双面板设计：
 * 
 * 左侧边栏（固定宽度约320px）：
 *   - 输入配置：视频和模型文件选择
 *   - 分析控制：开始按钮
 *   - 状态/进度：实时更新、已用时间、进度条
 * 
 * 右侧主区域（可扩展）：
 *   - 分析日志：来自Python进程的实时stdout/stderr
 *   - 结果选项卡：摘要、数据表（CSV/JSON）、视频输出
 * 
 * 所有信号/槽连接都在此处建立以连接事件处理程序。
 ******************************************************************************/
void MainWindow::setupUI()
{
    // 创建仪表板布局的主分割器
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setChildrenCollapsible(false);
    setCentralWidget(mainSplitter);
    
    // ===== 左侧边栏（固定约320px） =====
    QWidget *leftSidebar = new QWidget(this);
    leftSidebar->setProperty("sidebar", true);
    leftSidebar->setMinimumWidth(280);
    leftSidebar->setMaximumWidth(400);
    
    QVBoxLayout *sidebarLayout = new QVBoxLayout(leftSidebar);
    sidebarLayout->setSpacing(16);
    sidebarLayout->setContentsMargins(12, 12, 12, 12);
    
    // 输入配置部分
    QGroupBox *inputGroup = new QGroupBox("Input Configuration", this);
    inputGroup->setProperty("sidebarCard", true);
    
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
    inputLayout->setSpacing(12);
    inputLayout->setContentsMargins(16, 20, 16, 16);
    
    // 视频文件
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
    
    // YOLO模型
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
    
    // 分析控制部分
    QGroupBox *controlGroup = new QGroupBox("Analysis Control", this);
    controlGroup->setProperty("sidebarCard", true);
    
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    controlLayout->setSpacing(12);
    controlLayout->setContentsMargins(16, 20, 16, 16);
    
    // 主要行动号召 - 开始分析按钮
    startButton = new QPushButton("Start Analysis", this);
    startButton->setProperty("primary", true);
    startButton->setMinimumHeight(50);
    startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    controlLayout->addWidget(startButton);
    
    // 进度条（初始隐藏）
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 0);  // 不确定模式
    progressBar->setTextVisible(false);
    progressBar->setMinimumHeight(20);
    progressBar->setVisible(false);  // 初始隐藏
    progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    controlLayout->addWidget(progressBar);
    
    // 已用时间标签（初始隐藏）
    elapsedTimeLabel = new QLabel("Elapsed: 0:00", this);
    elapsedTimeLabel->setProperty("elapsedTime", true);
    elapsedTimeLabel->setAlignment(Qt::AlignCenter);
    elapsedTimeLabel->setVisible(false);  // 初始隐藏
    elapsedTimeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    controlLayout->addWidget(elapsedTimeLabel);
    
    sidebarLayout->addWidget(controlGroup);
    
    // 初始化定时器
    elapsedTimer = new QElapsedTimer();
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateElapsedTime);
    
    // 状态/进度部分
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
    
    // 添加弹性空间以将所有内容推到顶部
    sidebarLayout->addStretch(1);
    
    mainSplitter->addWidget(leftSidebar);
    
    // ===== 右侧主区域（选项卡部件） =====
    QWidget *mainArea = new QWidget(this);
    QVBoxLayout *mainAreaLayout = new QVBoxLayout(mainArea);
    mainAreaLayout->setSpacing(0);
    mainAreaLayout->setContentsMargins(0, 0, 0, 0);
    
    resultsTabWidget = new QTabWidget(this);
    resultsTabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 选项卡1：摘要（带空状态）
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
    
    // 选项卡2：数据表
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
    
    // 选项卡3：视频输出
    videoTab = new QWidget();
    videoTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *videoLayout = new QVBoxLayout(videoTab);
    videoLayout->setContentsMargins(16, 16, 16, 16);
    videoLayout->setSpacing(12);
    
    videoWidget = new QVideoWidget(this);
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoWidget->setMinimumHeight(300);
    videoLayout->addWidget(videoWidget, 1);
    
    // 视频控制
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
    
    // 选项卡4：日志
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
    
    // 设置分割器大小（左侧边栏约320px，其余为主区域）
    mainSplitter->setStretchFactor(0, 0);  // 侧边栏不拉伸
    mainSplitter->setStretchFactor(1, 1);  // 主区域拉伸
    mainSplitter->setSizes(QList<int>() << 320 << 880);
    
    // 连接清除日志按钮
    connect(clearLogButton, &QPushButton::clicked, [this]() {
        outputTextEdit->clear();
    });
    
    // 初始化媒体播放器
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);
    
    // 创建状态栏
    QStatusBar *statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    
    QLabel *statusBarLabel = new QLabel("Ready", this);
    statusBarLabel->setStyleSheet("padding: 4px; font-size: 9pt;");
    statusBar->addWidget(statusBarLabel);
    
    QLabel *versionLabel = new QLabel("v1.0.0", this);
    versionLabel->setStyleSheet("padding: 4px; font-size: 9pt; color: #666;");
    statusBar->addPermanentWidget(versionLabel);
    
    // 连接信号
    connect(browseInputButton, &QToolButton::clicked, this, &MainWindow::onBrowseInputVideo);
    connect(browseModelButton, &QToolButton::clicked, this, &MainWindow::onBrowseModel);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartAnalysis);
    connect(playPauseButton, &QPushButton::clicked, this, &MainWindow::onPlayPauseVideo);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopVideo);
}

/******************************************************************************
 * 事件处理程序：浏览输入视频
 * 
 * 打开文件对话框供用户选择输入视频文件。
 * 支持常见视频格式（MP4、AVI、MOV、MKV）。
 * 使用所选文件路径更新输入路径文本字段。
 ******************************************************************************/
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

/******************************************************************************
 * 事件处理程序：浏览模型
 * 
 * 打开文件对话框供用户选择YOLO模型文件。
 * 支持PyTorch模型格式（.pt、.pth）。
 * 使用所选文件路径更新模型路径文本字段。
 ******************************************************************************/
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

/******************************************************************************
 * 事件处理程序：开始分析
 * 
 * 作为单独的进程启动Python视频分析管道。
 * 
 * 验证：
 * - 检查分析是否已在运行
 * - 验证是否提供了输入视频和模型路径
 * - 在开始前验证文件是否存在
 * 
 * 进程执行：
 * - 从UI清除之前的结果
 * - 为Python脚本构造命令行参数
 * - 将工作目录更改为foot-Function
 * - 启动QProcess异步运行Python main.py
 * - 连接进程信号以进行实时监控
 * 
 * UI更新：
 * - 分析期间禁用开始按钮
 * - 启动已用时间计数器
 * - 将状态标签更新为"正在运行"
 * - 显示初始日志消息
 ******************************************************************************/
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
    
    // 清除之前的结果
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
    
    // 如果需要，初始化进程
    if (!pythonProcess) {
        pythonProcess = new QProcess(this);
        connect(pythonProcess, &QProcess::readyReadStandardOutput, 
                this, &MainWindow::onProcessReadyReadStandardOutput);
        connect(pythonProcess, &QProcess::readyReadStandardError, 
                this, &MainWindow::onProcessReadyReadStandardError);
        connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &MainWindow::onProcessFinished);
    }
    
    // 设置Python命令
    QString projectRoot = getProjectRootPath();
    QString scriptPath = QDir(projectRoot).absoluteFilePath("foot-Function/main.py");
    
    if (!QFileInfo::exists(scriptPath)) {
        QMessageBox::critical(this, "Script Not Found", 
            QString("Python script not found at: %1\n\nMake sure the foot-Function directory is present in the project root.").arg(scriptPath));
        return;
    }
    
    QStringList arguments;
    arguments << scriptPath;
    arguments << "--input" << inputVideo;
    arguments << "--model" << modelPath;
    
    // 启动进程
    QString workingDir = QDir(projectRoot).absoluteFilePath("foot-Function");
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
    
    // 显示并启动进度指示器
    progressBar->setVisible(true);
    progressBar->setRange(0, 0);  // 不确定模式
    
    elapsedTimer->start();
    updateTimer->start(1000);  // 每秒更新一次
    elapsedTimeLabel->setVisible(true);
    elapsedTimeLabel->setText("Elapsed: 0:00");
    
    outputTextEdit->append("=== Analysis Started ===\n");
    outputTextEdit->append(QString("Command: python %1\n").arg(arguments.join(" ")));
}

/******************************************************************************
 * 事件处理程序：进程准备读取标准输出
 * 
 * 当Python进程写入stdout时自动调用。
 * 实时捕获输出并在分析日志中显示。
 * 自动滚动到底部以显示最新输出。
 * 
 * 这在分析期间为用户提供实时反馈。
 ******************************************************************************/
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

/******************************************************************************
 * 事件处理程序：进程准备读取标准错误
 * 
 * 当Python进程写入stderr时自动调用。
 * 实时捕获错误输出并在分析日志中以红色显示。
 * 自动滚动到底部以显示最新输出。
 * 
 * 这帮助用户在分析期间识别问题。
 ******************************************************************************/
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

/******************************************************************************
 * 事件处理程序：进程完成
 * 
 * 当Python分析进程完成时调用（成功或失败）。
 * 
 * 完成处理：
 * - 停止已用时间计数器和进度指示器
 * - 重新启用开始按钮以进行下一次分析
 * - 根据退出代码更新状态标签
 * 
 * 结果加载（成功时）：
 * - 在foot-Function/output_videos/中搜索输出文件
 * - 将CSV数据加载到表格部件（或将JSON作为备用）
 * - 将输出视频加载到媒体播放器
 * - 切换到数据表选项卡以显示结果
 * 
 * 错误处理（失败时）：
 * - 显示错误状态
 * - 在日志中显示详细错误消息
 ******************************************************************************/
void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    analysisRunning = false;
    startButton->setEnabled(true);
    
    // 隐藏并停止进度指示器
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
        
        // 从项目根目录获取输出目录
        QString projectRoot = getProjectRootPath();
        QString outputDirPath = QDir(projectRoot).absoluteFilePath("foot-Function/output_videos");
        
        // 加载CSV数据
        QString csvPath = QDir(outputDirPath).absoluteFilePath("data_output.csv");
        if (QFileInfo::exists(csvPath)) {
            loadAndDisplayCSV(csvPath);
            outputTextEdit->append(QString("Loaded CSV data from: %1").arg(csvPath));
        }
        
        // 加载JSON数据（如果CSV失败则作为备用）
        QString jsonPath = QDir(outputDirPath).absoluteFilePath("data_output.json");
        if (QFileInfo::exists(jsonPath) && dataTableWidget->rowCount() == 0) {
            loadAndDisplayJSON(jsonPath);
            outputTextEdit->append(QString("Loaded JSON data from: %1").arg(jsonPath));
        }
        
        // 加载并播放视频
        QString videoPath = QDir(outputDirPath).absoluteFilePath("output_video.avi");
        if (QFileInfo::exists(videoPath)) {
            loadAndPlayVideo(videoPath);
            outputTextEdit->append(QString("Loaded video from: %1").arg(videoPath));
        }
        
        // 尝试查找并显示摘要选项卡的输出
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
    // 在foot-Function/output_videos目录中查找输出
    QString projectRoot = getProjectRootPath();
    QString outputDirPath = QDir(projectRoot).absoluteFilePath("foot-Function/output_videos");
    QDir outputDir(outputDirPath);
    
    if (!outputDir.exists()) {
        qDebug() << "Output directory does not exist:" << outputDir.path();
        return QString();
    }
    
    // 查找视频文件（avi、mp4）或图像文件（png、jpg）
    QStringList filters;
    filters << "*.avi" << "*.mp4" << "*.png" << "*.jpg" << "*.jpeg";
    
    QFileInfoList files = outputDir.entryInfoList(filters, QDir::Files, QDir::Time);
    
    if (!files.isEmpty()) {
        // 返回最近修改的文件
        QString path = files.first().absoluteFilePath();
        qDebug() << "Found output file:" << path;
        return path;
    }
    
    qDebug() << "No output files found in:" << outputDir.path();
    return QString();
}

/******************************************************************************
 * 旧方法：显示结果媒体
 * 
 * 用于显示结果文件（图像或视频路径）的旧方法。
 * 
 * 对于视频文件：以文本形式显示路径信息
 * 对于图像文件：加载并缩放显示图像
 * 
 * 注意：此方法主要被专用视频播放器和数据表显示方法所取代，
 * 但为了兼容性而保留。
 ******************************************************************************/
void MainWindow::displayResultMedia(const QString &mediaPath)
{
    if (mediaPath.isEmpty() || !QFileInfo::exists(mediaPath)) {
        resultImageLabel->setText("Output file not found.");
        return;
    }
    
    lastOutputPath = mediaPath;
    QFileInfo fileInfo(mediaPath);
    QString extension = fileInfo.suffix().toLower();
    
    // 对于视频文件，只显示带路径的消息
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
    
    // 对于图像文件，显示图像
    if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "bmp") {
        QPixmap pixmap(mediaPath);
        
        if (pixmap.isNull()) {
            resultImageLabel->setText("Failed to load image.");
            return;
        }
        
        // 缩放图像以适应同时保持宽高比
        QPixmap scaledPixmap = pixmap.scaled(
            resultScrollArea->viewport()->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        
        resultImageLabel->setProperty("emptyState", false);  // 删除空状态属性
        resultImageLabel->setPixmap(scaledPixmap);
        resultImageLabel->setText("");
        return;
    }
    
    // 未知文件类型
    resultImageLabel->setProperty("emptyState", false);  // 删除空状态属性
    resultImageLabel->setText(
        QString("Analysis complete!\n\nOutput saved to:\n%1").arg(mediaPath)
    );
}

/******************************************************************************
 * 数据加载方法：加载并显示CSV
 * 
 * 解析CSV输出文件并在表格部件中显示数据。
 * 
 * 预期CSV格式：
 * - 带列名的标题行
 * - 带球员统计数据和球队控球率的数据行
 * 
 * 解析逻辑：
 * - 逐行读取CSV
 * - 按逗号分割以提取单元格值
 * - 处理引号字段（用于值内的逗号）
 * - 使用解析的数据填充QTableWidget
 * 
 * UI更新：
 * - 根据CSV标题配置表格列
 * - 用球员/球队数据填充行
 * - 启用排序和选择
 ******************************************************************************/
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
    
    // 解析CSV - 处理基本的逗号分隔
    // 注意：这期望简单的CSV，不带引号字段（由Python脚本生成）
    QStringList headers = rows[0].split(',');
    dataTableWidget->setColumnCount(headers.size());
    dataTableWidget->setHorizontalHeaderLabels(headers);
    
    // 设置行
    dataTableWidget->setRowCount(rows.size() - 1);
    
    for (int i = 1; i < rows.size(); ++i) {
        QStringList columns = rows[i].split(',');
        for (int j = 0; j < columns.size() && j < headers.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(columns[j].trimmed());
            dataTableWidget->setItem(i - 1, j, item);
        }
    }
    
    // 调整列大小以适应内容
    dataTableWidget->resizeColumnsToContents();
    
    // 切换到数据选项卡
    resultsTabWidget->setCurrentWidget(dataTab);
}

/******************************************************************************
 * 数据加载方法：加载并显示JSON
 * 
 * 解析JSON输出文件并在表格部件中显示数据。
 * 当CSV不可用时用作备用方案。
 * 
 * 预期JSON格式：
 * - 包含球员数据和球队统计数据的根对象
 * - 带嵌套对象/数组的灵活结构
 * 
 * 解析逻辑：
 * - 使用QJsonDocument解析JSON
 * - 提取球员统计数据和球队控球数据
 * - 将JSON结构转换为表格格式
 * 
 * 当CSV解析失败时，此方法提供替代数据视图。
 ******************************************************************************/
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
    
    // 设置表格标题
    dataTableWidget->setColumnCount(3);
    dataTableWidget->setHorizontalHeaderLabels(QStringList() << "Team" << "Player ID" << "Distance (m)");
    
    int row = 0;
    
    // 处理每个球队
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
    
    // 添加摘要行
    if (root.contains("summary")) {
        QJsonObject summary = root["summary"].toObject();
        
        // 添加空行以分隔
        dataTableWidget->insertRow(row);
        row++;
        
        // 添加摘要标题
        dataTableWidget->insertRow(row);
        QTableWidgetItem *headerItem = new QTableWidgetItem("Summary - Team Possession Percentage");
        QFont boldFont;
        boldFont.setBold(true);
        headerItem->setFont(boldFont);
        dataTableWidget->setItem(row, 0, headerItem);
        row++;
        
        // 添加控球百分比
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
    
    // 调整列大小以适应内容
    dataTableWidget->resizeColumnsToContents();
    
    // 切换到数据选项卡
    resultsTabWidget->setCurrentWidget(dataTab);
}

/******************************************************************************
 * 视频加载方法：加载并播放视频
 * 
 * 将标注的输出视频加载到Qt多媒体播放器中。
 * 
 * 功能：
 * - 验证视频文件是否存在
 * - 在QMediaPlayer中设置视频源
 * - 启用播放控制按钮
 * - 切换到视频输出选项卡
 * 
 * 视频通常包含：
 * - 带球队颜色的球员边界框
 * - 球追踪覆盖层
 * - 速度和距离指标
 * - 控球指示器
 ******************************************************************************/
void MainWindow::loadAndPlayVideo(const QString &videoPath)
{
    if (!QFileInfo::exists(videoPath)) {
        qDebug() << "Video file does not exist:" << videoPath;
        return;
    }
    
    mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));
    playPauseButton->setEnabled(true);
    stopButton->setEnabled(true);
    
    // 切换到视频选项卡
    resultsTabWidget->setCurrentWidget(videoTab);
}

/******************************************************************************
 * 事件处理程序：播放/暂停视频
 * 
 * 在播放和暂停状态之间切换视频播放。
 * 更新按钮文本以反映当前状态。
 ******************************************************************************/
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

/******************************************************************************
 * 事件处理程序：停止视频
 * 
 * 停止视频播放并重置到开始位置。
 * 将按钮文本更新为"播放"状态。
 ******************************************************************************/
void MainWindow::onStopVideo()
{
    mediaPlayer->stop();
    playPauseButton->setText("Play");
}

/******************************************************************************
 * 定时器回调：更新已用时间
 * 
 * QTimer每秒调用一次以更新已用时间显示。
 * 将时间格式化为"M:SS"以提高可读性。
 * 持续到分析完成。
 ******************************************************************************/
void MainWindow::updateElapsedTime()
{
    if (elapsedTimer->isValid()) {
        qint64 elapsed = elapsedTimer->elapsed();  // 毫秒
        int seconds = elapsed / 1000;
        int minutes = seconds / 60;
        seconds = seconds % 60;
        
        QString timeText = QString("Elapsed: %1:%2")
            .arg(minutes)
            .arg(seconds, 2, 10, QChar('0'));
        elapsedTimeLabel->setText(timeText);
    }
}

/******************************************************************************
 * UI设置方法：加载样式表
 * 
 * 加载并应用QSS（Qt样式表）以实现现代、专业的样式。
 * 从项目根目录中的modern_style.qss文件读取。
 * 
 * 样式表提供：
 * - 带阴影和边框的卡片式UI设计
 * - 颜色编码的状态指示器
 * - 一致的间距和排版
 * - 专业的配色方案
 * 
 * 优雅地处理缺失的样式表文件。
 ******************************************************************************/
void MainWindow::loadStyleSheet()
{
    QFile styleFile(":/modern_style.qss");

    if (!styleFile.exists()) {
        styleFile.setFileName("modern_style.qss");
    }

    if (!styleFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Failed to open stylesheet file";
        return;
    }

    QString styleSheet = QString::fromUtf8(styleFile.readAll());
    styleFile.close();

    // ✔ 这里一定要转换成 QApplication*
    if (QApplication *app = qobject_cast<QApplication*>(QApplication::instance())) {
        app->setStyleSheet(styleSheet);
        qDebug() << "Stylesheet applied successfully";
    } else {
        qDebug() << "QApplication instance not found. Cannot set stylesheet.";
    }
}
