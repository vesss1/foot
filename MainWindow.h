#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QPixmap>
#include <QGroupBox>

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

private:
    void setupUI();
    void displayResultMedia(const QString &mediaPath);
    QString findOutputVideo();
    
    // UI Components
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    
    // Input controls
    QLineEdit *inputVideoPathEdit;
    QPushButton *browseInputButton;
    QLineEdit *modelPathEdit;
    QPushButton *browseModelButton;
    QPushButton *startButton;
    
    // Output display
    QTextEdit *outputTextEdit;
    QLabel *statusLabel;
    
    // Results display
    QLabel *resultImageLabel;
    QScrollArea *resultScrollArea;
    QGroupBox *resultsGroupBox;
    
    // Process
    QProcess *pythonProcess;
    
    // State
    QString lastOutputPath;
    bool analysisRunning;
};

#endif // MAINWINDOW_H
