#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QString>

/**
 * @brief MainWindow class for Football Analysis GUI
 * 
 * This class provides a Qt GUI interface to the existing foot-Function Python pipeline.
 * It uses QProcess to execute the Python script without modifying the original code.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // Constants
    static constexpr int SEPARATOR_LENGTH = 60;
    static constexpr int PROCESS_START_TIMEOUT_MS = 5000;
    static constexpr int PROCESS_TERMINATE_TIMEOUT_MS = 5000;
    static constexpr int PROCESS_KILL_TIMEOUT_MS = 3000;

private slots:
    /**
     * @brief Handle Browse button for input video selection
     */
    void onBrowseInputVideo();
    
    /**
     * @brief Handle Browse button for model file selection
     */
    void onBrowseModelFile();
    
    /**
     * @brief Handle Browse button for output directory selection
     */
    void onBrowseOutputDir();
    
    /**
     * @brief Handle Run Analysis button click
     * Starts the Python script via QProcess
     */
    void onRunAnalysis();
    
    /**
     * @brief Handle Stop button click
     * Terminates the running process
     */
    void onStopAnalysis();
    
    /**
     * @brief Capture and display stdout from the Python process
     */
    void onProcessReadyReadStdout();
    
    /**
     * @brief Capture and display stderr from the Python process
     */
    void onProcessReadyReadStderr();
    
    /**
     * @brief Handle process completion
     * @param exitCode Process exit code
     * @param exitStatus Process exit status
     */
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    
    /**
     * @brief Handle process errors
     * @param error Process error type
     */
    void onProcessError(QProcess::ProcessError error);
    
    /**
     * @brief Handle process state changes
     * @param state New process state
     */
    void onProcessStateChanged(QProcess::ProcessState state);

private:
    /**
     * @brief Initialize the user interface
     */
    void setupUI();
    
    /**
     * @brief Create the input configuration group
     * @return QGroupBox with input configuration widgets
     */
    QGroupBox* createInputGroup();
    
    /**
     * @brief Create the output display group
     * @return QGroupBox with output display widgets
     */
    QGroupBox* createOutputGroup();
    
    /**
     * @brief Create the control buttons
     * @return QHBoxLayout with control buttons
     */
    QHBoxLayout* createControlButtons();
    
    /**
     * @brief Validate input parameters before running
     * @return true if all inputs are valid, false otherwise
     */
    bool validateInputs();
    
    /**
     * @brief Append text to the output console
     * @param text Text to append
     * @param isError true for error messages (red), false for normal output
     */
    void appendOutput(const QString& text, bool isError = false);
    
    /**
     * @brief Enable or disable UI controls
     * @param enabled true to enable, false to disable
     */
    void setControlsEnabled(bool enabled);
    
    /**
     * @brief Find Python interpreter with OpenCV installed
     * @return Path to Python interpreter, or empty string if not found
     */
    QString findPythonWithOpenCV();

    // UI Components
    QLineEdit* m_inputVideoEdit;
    QLineEdit* m_modelFileEdit;
    QLineEdit* m_outputDirEdit;
    QCheckBox* m_useStubsCheckbox;
    
    QPushButton* m_browseInputBtn;
    QPushButton* m_browseModelBtn;
    QPushButton* m_browseOutputBtn;
    QPushButton* m_runBtn;
    QPushButton* m_stopBtn;
    
    QTextEdit* m_outputConsole;
    QLabel* m_statusLabel;
    
    // Process management
    QProcess* m_process;
    
    // Configuration
    QString m_footFunctionPath;
    QString m_pythonExecutable;
};

#endif // MAINWINDOW_H
