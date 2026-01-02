#include "VideoDataViewer.h"
#include <QFileInfo>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QHeaderView>
#include <QSplitter>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

VideoDataViewer::VideoDataViewer(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , videoLabel(nullptr)
    , videoGroupBox(nullptr)
    , playPauseButton(nullptr)
    , resetButton(nullptr)
    , frameInfoLabel(nullptr)
    , dataGroupBox(nullptr)
    , dataDisplayCombo(nullptr)
    , dataTable(nullptr)
    , dataTextEdit(nullptr)
    , frameTimer(nullptr)
    , currentFrameIndex(0)
    , totalFrames(0)
    , fps(30.0)
    , isPlaying(true)
    , isCSV(false)
{
    setupUI();
    setWindowTitle("Video and Data Viewer - Qt 6.10.1");
    resize(1200, 900);
    
    // Initialize timer for video playback
    frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, this, &VideoDataViewer::updateFrame);
    
    // Try to load video and data from default location
    QString outputDir = QDir::currentPath() + "/foot-Function/output_videos";
    
    // Check if directory exists
    if (QDir(outputDir).exists()) {
        QString videoPath = outputDir + "/output.avi";
        QString dataPath = findDataFile(outputDir);
        
        if (QFileInfo::exists(videoPath)) {
            if (loadVideo(videoPath)) {
                qDebug() << "Video loaded successfully:" << videoPath;
                if (!dataPath.isEmpty() && loadDataFile(dataPath)) {
                    qDebug() << "Data file loaded successfully:" << dataPath;
                }
                // Start playback
                frameTimer->start(static_cast<int>(1000.0 / fps));
            }
        } else {
            videoLabel->setText("Video file 'output.avi' not found in:\n" + outputDir + 
                              "\n\nPlease ensure the output files exist.");
        }
    } else {
        videoLabel->setText("Output directory not found:\n" + outputDir + 
                          "\n\nPlease run the analysis first to generate output files.");
    }
}

VideoDataViewer::~VideoDataViewer()
{
    if (videoCapture.isOpened()) {
        videoCapture.release();
    }
}

void VideoDataViewer::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Create a splitter for resizable sections
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    
    // Video Section
    videoGroupBox = new QGroupBox("Video Playback", this);
    QVBoxLayout *videoLayout = new QVBoxLayout(videoGroupBox);
    
    videoLabel = new QLabel(this);
    videoLabel->setAlignment(Qt::AlignCenter);
    videoLabel->setMinimumSize(640, 480);
    videoLabel->setStyleSheet("QLabel { background-color: #000000; color: white; }");
    videoLabel->setText("Loading video...");
    videoLayout->addWidget(videoLabel);
    
    // Video controls
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    playPauseButton = new QPushButton("Pause", this);
    resetButton = new QPushButton("Reset", this);
    frameInfoLabel = new QLabel("Frame: 0 / 0", this);
    
    controlsLayout->addWidget(playPauseButton);
    controlsLayout->addWidget(resetButton);
    controlsLayout->addStretch();
    controlsLayout->addWidget(frameInfoLabel);
    
    videoLayout->addLayout(controlsLayout);
    splitter->addWidget(videoGroupBox);
    
    // Data Section
    dataGroupBox = new QGroupBox("Data Display", this);
    QVBoxLayout *dataLayout = new QVBoxLayout(dataGroupBox);
    
    // Data display mode selector
    QHBoxLayout *dataControlLayout = new QHBoxLayout();
    dataControlLayout->addWidget(new QLabel("Display Mode:", this));
    dataDisplayCombo = new QComboBox(this);
    dataDisplayCombo->addItem("Table View");
    dataDisplayCombo->addItem("Raw Text");
    dataControlLayout->addWidget(dataDisplayCombo);
    dataControlLayout->addStretch();
    dataLayout->addLayout(dataControlLayout);
    
    // Table widget for structured data
    dataTable = new QTableWidget(this);
    dataTable->setAlternatingRowColors(true);
    dataTable->horizontalHeader()->setStretchLastSection(true);
    dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dataLayout->addWidget(dataTable);
    
    // Text edit for raw data
    dataTextEdit = new QTextEdit(this);
    dataTextEdit->setReadOnly(true);
    dataTextEdit->setVisible(false);
    dataLayout->addWidget(dataTextEdit);
    
    splitter->addWidget(dataGroupBox);
    
    // Set initial splitter sizes (60% video, 40% data)
    splitter->setStretchFactor(0, 60);
    splitter->setStretchFactor(1, 40);
    
    mainLayout->addWidget(splitter);
    
    // Connect signals
    connect(playPauseButton, &QPushButton::clicked, this, &VideoDataViewer::onPlayPauseClicked);
    connect(resetButton, &QPushButton::clicked, this, &VideoDataViewer::onResetClicked);
    connect(dataDisplayCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &VideoDataViewer::onDataDisplayModeChanged);
}

bool VideoDataViewer::loadVideo(const QString &videoPath)
{
    if (!QFileInfo::exists(videoPath)) {
        qDebug() << "Video file does not exist:" << videoPath;
        return false;
    }
    
    videoCapture.open(videoPath.toStdString());
    
    if (!videoCapture.isOpened()) {
        qDebug() << "Failed to open video:" << videoPath;
        videoLabel->setText("Failed to open video file:\n" + videoPath);
        return false;
    }
    
    totalFrames = static_cast<int>(videoCapture.get(cv::CAP_PROP_FRAME_COUNT));
    fps = videoCapture.get(cv::CAP_PROP_FPS);
    if (fps <= 0) {
        fps = 30.0; // Default fallback
    }
    
    currentFrameIndex = 0;
    
    qDebug() << "Video loaded - Total frames:" << totalFrames << "FPS:" << fps;
    
    // Read and display first frame
    if (videoCapture.read(currentFrame)) {
        displayFrame(currentFrame);
        currentFrameIndex = 1;
        frameInfoLabel->setText(QString("Frame: %1 / %2").arg(currentFrameIndex).arg(totalFrames));
    }
    
    return true;
}

QString VideoDataViewer::findDataFile(const QString &outputDir)
{
    QDir dir(outputDir);
    
    // Look for CSV files first
    QStringList csvFiles = dir.entryList(QStringList() << "*.csv", QDir::Files);
    if (!csvFiles.isEmpty()) {
        return dir.absoluteFilePath(csvFiles.first());
    }
    
    // Then look for JSON files
    QStringList jsonFiles = dir.entryList(QStringList() << "*.json", QDir::Files);
    if (!jsonFiles.isEmpty()) {
        return dir.absoluteFilePath(jsonFiles.first());
    }
    
    return QString();
}

bool VideoDataViewer::loadDataFile(const QString &dataPath)
{
    if (dataPath.isEmpty() || !QFileInfo::exists(dataPath)) {
        qDebug() << "Data file does not exist:" << dataPath;
        return false;
    }
    
    QFileInfo fileInfo(dataPath);
    QString extension = fileInfo.suffix().toLower();
    
    if (extension == "csv") {
        loadCSVData(dataPath);
        isCSV = true;
        return true;
    } else if (extension == "json") {
        loadJSONData(dataPath);
        isCSV = false;
        return true;
    }
    
    return false;
}

void VideoDataViewer::loadCSVData(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open CSV file:" << filePath;
        return;
    }
    
    QTextStream in(&file);
    dataContent = in.readAll();
    file.close();
    
    // Parse CSV
    QStringList lines = dataContent.split('\n', Qt::SplitBehaviorFlags::SkipEmptyParts);
    
    if (lines.isEmpty()) {
        return;
    }
    
    // First line is header
    dataHeaders = lines.first().split(',');
    
    // Remove header from lines
    lines.removeFirst();
    
    // Parse data rows
    dataRows.clear();
    for (const QString &line : lines) {
        if (line.trimmed().isEmpty()) continue;
        dataRows.append(line.split(','));
    }
    
    updateDataDisplay();
    qDebug() << "CSV loaded - Rows:" << dataRows.size() << "Columns:" << dataHeaders.size();
}

void VideoDataViewer::loadJSONData(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file:" << filePath;
        return;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    dataContent = QString::fromUtf8(jsonData);
    
    // Try to parse JSON and convert to table format
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << error.errorString();
        // Still show raw content
        updateDataDisplay();
        return;
    }
    
    // If it's an array of objects, convert to table
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        if (!array.isEmpty() && array.first().isObject()) {
            QJsonObject firstObj = array.first().toObject();
            
            // Extract headers from first object keys
            dataHeaders = firstObj.keys();
            
            // Extract data rows
            dataRows.clear();
            for (const QJsonValue &value : array) {
                if (value.isObject()) {
                    QJsonObject obj = value.toObject();
                    QStringList row;
                    for (const QString &key : dataHeaders) {
                        QJsonValue val = obj.value(key);
                        if (val.isString()) {
                            row.append(val.toString());
                        } else if (val.isDouble()) {
                            row.append(QString::number(val.toDouble()));
                        } else if (val.isBool()) {
                            row.append(val.toBool() ? "true" : "false");
                        } else if (val.isArray()) {
                            row.append(QJsonDocument(val.toArray()).toJson(QJsonDocument::Compact));
                        } else if (val.isObject()) {
                            row.append(QJsonDocument(val.toObject()).toJson(QJsonDocument::Compact));
                        } else {
                            row.append("null");
                        }
                    }
                    dataRows.append(row);
                }
            }
        }
    } else if (doc.isObject()) {
        // If it's a single object, show as key-value pairs
        QJsonObject obj = doc.object();
        dataHeaders = QStringList() << "Key" << "Value";
        dataRows.clear();
        
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            QStringList row;
            row.append(it.key());
            
            QJsonValue val = it.value();
            if (val.isString()) {
                row.append(val.toString());
            } else if (val.isDouble()) {
                row.append(QString::number(val.toDouble()));
            } else if (val.isBool()) {
                row.append(val.toBool() ? "true" : "false");
            } else if (val.isArray()) {
                row.append(QJsonDocument(val.toArray()).toJson(QJsonDocument::Compact));
            } else if (val.isObject()) {
                row.append(QJsonDocument(val.toObject()).toJson(QJsonDocument::Compact));
            } else {
                row.append("null");
            }
            
            dataRows.append(row);
        }
    }
    
    updateDataDisplay();
    qDebug() << "JSON loaded - Rows:" << dataRows.size();
}

void VideoDataViewer::updateDataDisplay()
{
    // Update text view
    dataTextEdit->setPlainText(dataContent);
    
    // Update table view
    if (dataHeaders.isEmpty() || dataRows.isEmpty()) {
        dataTable->clear();
        dataTable->setRowCount(0);
        dataTable->setColumnCount(0);
        return;
    }
    
    dataTable->setColumnCount(dataHeaders.size());
    dataTable->setHorizontalHeaderLabels(dataHeaders);
    dataTable->setRowCount(dataRows.size());
    
    for (int row = 0; row < dataRows.size(); ++row) {
        const QStringList &rowData = dataRows[row];
        for (int col = 0; col < qMin(rowData.size(), dataHeaders.size()); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            dataTable->setItem(row, col, item);
        }
    }
    
    dataTable->resizeColumnsToContents();
}

void VideoDataViewer::updateFrame()
{
    if (!isPlaying || !videoCapture.isOpened()) {
        return;
    }
    
    if (videoCapture.read(currentFrame)) {
        displayFrame(currentFrame);
        currentFrameIndex++;
        frameInfoLabel->setText(QString("Frame: %1 / %2").arg(currentFrameIndex).arg(totalFrames));
    } else {
        // End of video - loop back to start
        videoCapture.set(cv::CAP_PROP_POS_FRAMES, 0);
        currentFrameIndex = 0;
        if (videoCapture.read(currentFrame)) {
            displayFrame(currentFrame);
            currentFrameIndex = 1;
            frameInfoLabel->setText(QString("Frame: %1 / %2").arg(currentFrameIndex).arg(totalFrames));
        }
    }
}

void VideoDataViewer::displayFrame(const cv::Mat &frame)
{
    if (frame.empty()) {
        return;
    }
    
    // Convert BGR to RGB
    cv::Mat rgbFrame;
    cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
    
    // Create QImage from cv::Mat
    QImage image(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, 
                 static_cast<int>(rgbFrame.step), QImage::Format_RGB888);
    
    // Scale to fit label while maintaining aspect ratio
    QPixmap pixmap = QPixmap::fromImage(image);
    QPixmap scaledPixmap = pixmap.scaled(videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    videoLabel->setPixmap(scaledPixmap);
}

void VideoDataViewer::onPlayPauseClicked()
{
    isPlaying = !isPlaying;
    playPauseButton->setText(isPlaying ? "Pause" : "Play");
}

void VideoDataViewer::onResetClicked()
{
    if (videoCapture.isOpened()) {
        videoCapture.set(cv::CAP_PROP_POS_FRAMES, 0);
        currentFrameIndex = 0;
        
        if (videoCapture.read(currentFrame)) {
            displayFrame(currentFrame);
            currentFrameIndex = 1;
            frameInfoLabel->setText(QString("Frame: %1 / %2").arg(currentFrameIndex).arg(totalFrames));
        }
    }
}

void VideoDataViewer::onDataDisplayModeChanged(int index)
{
    if (index == 0) {
        // Table view
        dataTable->setVisible(true);
        dataTextEdit->setVisible(false);
    } else {
        // Raw text view
        dataTable->setVisible(false);
        dataTextEdit->setVisible(true);
    }
}
