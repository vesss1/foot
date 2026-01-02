#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QString>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBrowsePython();
    void onBrowseScript();
    void onBrowseVideo();
    void onBrowseModel();
    void onBrowseOutput();
    void onRunAnalysis();
    void onStopAnalysis();
    void onProcessReadyReadStdout();
    void onProcessReadyReadStderr();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    void setupUI();
    void appendOutput(const QString& text, bool isError = false);
    void setControlsEnabled(bool enabled);
    bool validateInputs();
    void parseResults(const QString& outputDir);
    
    // UI Components
    QLineEdit* m_pythonPathEdit;
    QLineEdit* m_scriptPathEdit;
    QLineEdit* m_videoPathEdit;
    QLineEdit* m_modelPathEdit;
    QLineEdit* m_outputDirEdit;
    
    QPushButton* m_browsePythonBtn;
    QPushButton* m_browseScriptBtn;
    QPushButton* m_browseVideoBtn;
    QPushButton* m_browseModelBtn;
    QPushButton* m_browseOutputBtn;
    QPushButton* m_runBtn;
    QPushButton* m_stopBtn;
    
    QTextEdit* m_outputConsole;
    QLabel* m_statusLabel;
    QLabel* m_team1PossessionLabel;
    QLabel* m_team2PossessionLabel;
    QLabel* m_team1DistanceLabel;
    QLabel* m_team2DistanceLabel;
    QLabel* m_videoPlaceholder;
    
    QProcess* m_process;
};

#endif // MAINWINDOW_H
