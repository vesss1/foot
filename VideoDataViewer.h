#ifndef VIDEODATAVIEWER_H
#define VIDEODATAVIEWER_H

#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QGroupBox>
#include <QComboBox>
#include <QString>
#include <opencv2/opencv.hpp>

class VideoDataViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoDataViewer(QWidget *parent = nullptr);
    ~VideoDataViewer();

private slots:
    void updateFrame();
    void onPlayPauseClicked();
    void onResetClicked();
    void onDataDisplayModeChanged(int index);

private:
    void setupUI();
    bool loadVideo(const QString &videoPath);
    bool loadDataFile(const QString &dataPath);
    void displayFrame(const cv::Mat &frame);
    QString findDataFile(const QString &outputDir);
    void loadCSVData(const QString &filePath);
    void loadJSONData(const QString &filePath);
    void updateDataDisplay();

    // UI Components
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    
    // Video display
    QLabel *videoLabel;
    QGroupBox *videoGroupBox;
    
    // Video controls
    QPushButton *playPauseButton;
    QPushButton *resetButton;
    QLabel *frameInfoLabel;
    
    // Data display
    QGroupBox *dataGroupBox;
    QComboBox *dataDisplayCombo;
    QTableWidget *dataTable;
    QTextEdit *dataTextEdit;
    
    // Video playback
    cv::VideoCapture videoCapture;
    QTimer *frameTimer;
    cv::Mat currentFrame;
    int currentFrameIndex;
    int totalFrames;
    double fps;
    bool isPlaying;
    
    // Data storage
    QString dataContent;
    QStringList dataHeaders;
    QVector<QStringList> dataRows;
    bool isCSV;
};

#endif // VIDEODATAVIEWER_H
