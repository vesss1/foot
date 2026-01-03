#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QPixmap>
#include <QGroupBox>
#include <QTableWidget>
#include <QTabWidget>
#include <QHeaderView>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtMultimedia/QAudioOutput>
#include <QSplitter>
#include <QStatusBar>
#include <QToolButton>
#include <QProgressBar>
#include <QElapsedTimer>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBrowseInputVideo();
    void onBrowseModel();
    void onStartAnalysis();
    void onProcessReadyReadStandardOutput();
    void onProcessReadyReadStandardError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onPlayPauseVideo();
    void onStopVideo();
    void updateElapsedTime();

private:
    void setupUI();
    void loadStyleSheet();
    void displayResultMedia(const QString &mediaPath);
    QString findOutputVideo();
    void loadAndDisplayCSV(const QString &csvPath);
    void loadAndDisplayJSON(const QString &jsonPath);
    void loadAndPlayVideo(const QString &videoPath);
    QString getProjectRootPath() const;
    
    // UI Components
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    
    // Input controls
    QLineEdit *inputVideoPathEdit;
    QToolButton *browseInputButton;
    QLineEdit *modelPathEdit;
    QToolButton *browseModelButton;
    QPushButton *startButton;
    
    // Output display
    QTextEdit *outputTextEdit;
    QLabel *statusLabel;
    QProgressBar *progressBar;
    QLabel *elapsedTimeLabel;
    QElapsedTimer *elapsedTimer;
    QTimer *updateTimer;
    
    // Results display
    QTabWidget *resultsTabWidget;
    QLabel *resultImageLabel;
    QScrollArea *resultScrollArea;
    
    // Data display (CSV/JSON)
    QTableWidget *dataTableWidget;
    QWidget *dataTab;
    
    // Video playback
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QVideoWidget *videoWidget;
    QPushButton *playPauseButton;
    QPushButton *stopButton;
    QWidget *videoTab;
    
    // Process
    QProcess *pythonProcess;
    
    // State
    QString lastOutputPath;
    bool analysisRunning;
};

#endif // MAINWINDOW_H
