// EXAMPLE BUTTON LOGIC - Football Analysis Qt GUI
// This file demonstrates the key button handlers and their implementation

// ============================================================================
// EXAMPLE 1: Browse Input Video Button
// ============================================================================

void MainWindow::onBrowseInputVideo()
{
    // Open file dialog filtered for video files
    QString fileName = QFileDialog::getOpenFileName(
        this,                                    // Parent widget
        "Select Input Video",                    // Dialog title
        m_inputVideoEdit->text().isEmpty() ?     // Default directory
            m_footFunctionPath + "/input_videos" : 
            m_inputVideoEdit->text(),
        "Video Files (*.mp4 *.avi *.mov *.mkv);;All Files (*.*)"  // File filters
    );
    
    // Update the input field if user selected a file
    if (!fileName.isEmpty()) {
        m_inputVideoEdit->setText(fileName);
    }
}

// ============================================================================
// EXAMPLE 2: Run Analysis Button - Full Implementation
// ============================================================================

void MainWindow::onRunAnalysis()
{
    // Step 1: Validate all inputs before proceeding
    if (!validateInputs()) {
        return;  // Validation failed, error message already shown
    }
    
    // Step 2: Clear previous output and prepare UI
    m_outputConsole->clear();
    appendOutput("=".repeated(60), false);
    appendOutput("Starting Football Analysis...", false);
    appendOutput("Time: " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), false);
    appendOutput("=".repeated(60), false);
    
    // Step 3: Build Python wrapper script
    // This script imports the VideoAnalysisPipeline and runs it with user parameters
    QString wrapperScript = QString(
        "#!/usr/bin/env python3\n"
        "import sys\n"
        "sys.path.insert(0, '%1')\n"                    // Add foot-Function to path
        "from main import VideoAnalysisPipeline\n"
        "try:\n"
        "    pipeline = VideoAnalysisPipeline(\n"
        "        input_video_path='%2',\n"              // User's input video
        "        model_path='%3',\n"                    // User's model file
        "        output_dir='%4',\n"                    // User's output directory
        "        use_stubs=%5\n"                        // User's stub preference
        "    )\n"
        "    pipeline.run()\n"                          // Execute the analysis
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
    
    // Step 4: Write wrapper script to temporary file
    QString tempScriptPath = QDir::tempPath() + "/foot_analysis_wrapper.py";
    QFile tempFile(tempScriptPath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Failed to create temporary script file.");
        return;
    }
    
    QTextStream out(&tempFile);
    out << wrapperScript;
    tempFile.close();
    
    // Step 5: Display configuration
    appendOutput("Input Video: " + m_inputVideoEdit->text(), false);
    appendOutput("Model File: " + m_modelFileEdit->text(), false);
    appendOutput("Output Directory: " + m_outputDirEdit->text(), false);
    appendOutput("Use Stubs: " + QString(m_useStubsCheckbox->isChecked() ? "Yes" : "No"), false);
    appendOutput("", false);
    
    // Step 6: Configure QProcess
    m_process->setWorkingDirectory(m_footFunctionPath);
    
    // Step 7: Build command arguments
    QStringList arguments;
    arguments << tempScriptPath;
    
    // Step 8: Display execution command
    appendOutput("Command: python3 " + arguments.join(" "), false);
    appendOutput("Working Directory: " + m_footFunctionPath, false);
    appendOutput("=".repeated(60), false);
    appendOutput("", false);
    
    // Step 9: Start the process
    m_process->start("python3", arguments);
    
    // Step 10: Check if process started successfully
    if (!m_process->waitForStarted(5000)) {
        appendOutput("ERROR: Failed to start process!", true);
        QMessageBox::critical(this, "Error", 
            "Failed to start Python process.\n\n"
            "Make sure Python 3 is installed and accessible.");
    }
    
    // Note: Process output is handled by signal/slot connections
    // - onProcessReadyReadStdout() captures stdout
    // - onProcessReadyReadStderr() captures stderr
    // - onProcessFinished() handles completion
}

// ============================================================================
// EXAMPLE 3: Stop Button - Terminate Running Process
// ============================================================================

void MainWindow::onStopAnalysis()
{
    // Check if process is actually running
    if (m_process && m_process->state() != QProcess::NotRunning) {
        appendOutput("", false);
        appendOutput("=".repeated(60), false);
        appendOutput("Stopping process...", false);
        
        // Try graceful termination first
        m_process->terminate();
        
        // Wait up to 5 seconds for process to end
        if (!m_process->waitForFinished(5000)) {
            // Process didn't terminate, force kill
            appendOutput("Process did not terminate gracefully, killing...", false);
            m_process->kill();
        }
        
        // Process state change will trigger onProcessStateChanged()
        // which will re-enable controls
    }
}

// ============================================================================
// EXAMPLE 4: Stdout Capture - Real-time Output Display
// ============================================================================

void MainWindow::onProcessReadyReadStdout()
{
    // Read all available stdout data from process
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());
    
    // Display in output console (black text)
    appendOutput(output, false);
    
    // This is called automatically whenever stdout has new data
    // Examples of what might be captured:
    // - "2026-01-01 15:18:01 - INFO - Starting pipeline..."
    // - "Successfully read 300 frames"
    // - "Object tracking complete"
}

// ============================================================================
// EXAMPLE 5: Stderr Capture - Error Output Display
// ============================================================================

void MainWindow::onProcessReadyReadStderr()
{
    // Read all available stderr data from process
    QString output = QString::fromUtf8(m_process->readAllStandardError());
    
    // Display in output console (red text for errors)
    appendOutput(output, true);
    
    // This captures error messages like:
    // - "ERROR: Model file not found"
    // - "WARNING: Low memory available"
    // - Python traceback information
}

// ============================================================================
// EXAMPLE 6: Process Completion Handler
// ============================================================================

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    appendOutput("", false);
    appendOutput("=".repeated(60), false);
    
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            // Success case
            appendOutput("Process completed successfully!", false);
            m_statusLabel->setText("Analysis completed successfully");
            
            // Show success dialog with output location
            QMessageBox::information(this, "Success", 
                "Football analysis completed successfully!\n\n"
                "Output files saved to:\n" + m_outputDirEdit->text());
        } else {
            // Failed with error code
            appendOutput(QString("Process exited with code: %1").arg(exitCode), true);
            m_statusLabel->setText(QString("Process failed with exit code %1").arg(exitCode));
        }
    } else {
        // Process crashed or was killed
        appendOutput("Process crashed or was terminated!", true);
        m_statusLabel->setText("Process crashed or terminated");
    }
    
    appendOutput("=".repeated(60), false);
    
    // Re-enable UI controls
    setControlsEnabled(true);
}

// ============================================================================
// EXAMPLE 7: Input Validation Logic
// ============================================================================

bool MainWindow::validateInputs()
{
    // Check 1: Input video must be specified
    if (m_inputVideoEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", 
            "Please specify an input video file.");
        return false;
    }
    
    // Check 2: Input video file must exist
    QFileInfo inputVideo(m_inputVideoEdit->text());
    if (!inputVideo.exists() || !inputVideo.isFile()) {
        QMessageBox::warning(this, "Validation Error", 
            "Input video file does not exist:\n" + m_inputVideoEdit->text());
        return false;
    }
    
    // Check 3: Model file must be specified
    if (m_modelFileEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", 
            "Please specify a model file.");
        return false;
    }
    
    // Check 4: Model file must exist
    QFileInfo modelFile(m_modelFileEdit->text());
    if (!modelFile.exists() || !modelFile.isFile()) {
        QMessageBox::warning(this, "Validation Error", 
            "Model file does not exist:\n" + m_modelFileEdit->text());
        return false;
    }
    
    // Check 5: Output directory must be specified
    if (m_outputDirEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", 
            "Please specify an output directory.");
        return false;
    }
    
    // Check 6: Create output directory if it doesn't exist
    QDir outputDir(m_outputDirEdit->text());
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            QMessageBox::warning(this, "Validation Error", 
                "Failed to create output directory:\n" + m_outputDirEdit->text());
            return false;
        }
    }
    
    // Check 7: Verify foot-Function directory exists
    QDir footFunctionDir(m_footFunctionPath);
    if (!footFunctionDir.exists()) {
        QMessageBox::warning(this, "Validation Error", 
            "foot-Function directory not found:\n" + m_footFunctionPath + 
            "\n\nPlease ensure the application is in the correct location.");
        return false;
    }
    
    // Check 8: Verify main.py exists
    QFileInfo mainScript(m_footFunctionPath + "/main.py");
    if (!mainScript.exists()) {
        QMessageBox::warning(this, "Validation Error", 
            "main.py not found in foot-Function directory:\n" + m_footFunctionPath);
        return false;
    }
    
    // All validations passed
    return true;
}

// ============================================================================
// EXAMPLE 8: Helper Function - Append Output with Color Coding
// ============================================================================

void MainWindow::appendOutput(const QString& text, bool isError)
{
    // Handle empty lines
    if (text.isEmpty()) {
        m_outputConsole->append("");
        return;
    }
    
    // Set text format based on error flag
    QTextCharFormat format;
    if (isError) {
        format.setForeground(QBrush(QColor(255, 0, 0))); // Red for errors
    } else {
        format.setForeground(QBrush(QColor(0, 0, 0))); // Black for normal output
    }
    
    // Insert text at the end with appropriate formatting
    QTextCursor cursor = m_outputConsole->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text + "\n", format);
    
    // Auto-scroll to bottom to show latest output
    m_outputConsole->verticalScrollBar()->setValue(
        m_outputConsole->verticalScrollBar()->maximum()
    );
}

// ============================================================================
// EXAMPLE 9: UI Control State Management
// ============================================================================

void MainWindow::setControlsEnabled(bool enabled)
{
    // Enable/disable input controls
    m_inputVideoEdit->setEnabled(enabled);
    m_modelFileEdit->setEnabled(enabled);
    m_outputDirEdit->setEnabled(enabled);
    m_useStubsCheckbox->setEnabled(enabled);
    
    // Enable/disable browse buttons
    m_browseInputBtn->setEnabled(enabled);
    m_browseModelBtn->setEnabled(enabled);
    m_browseOutputBtn->setEnabled(enabled);
    
    // Run button: enabled when controls are enabled, disabled when running
    m_runBtn->setEnabled(enabled);
    
    // Stop button: opposite of other controls
    m_stopBtn->setEnabled(!enabled);
}

// ============================================================================
// EXAMPLE 10: Signal/Slot Connection Setup (in constructor)
// ============================================================================

// In MainWindow constructor:
void setupProcessConnections()
{
    // Create QProcess instance
    m_process = new QProcess(this);
    
    // Connect stdout signal
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onProcessReadyReadStdout);
    
    // Connect stderr signal
    connect(m_process, &QProcess::readyReadStandardError,
            this, &MainWindow::onProcessReadyReadStderr);
    
    // Connect process finished signal
    // Note: Using QOverload because finished() is overloaded
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onProcessFinished);
    
    // Connect error signal
    connect(m_process, &QProcess::errorOccurred,
            this, &MainWindow::onProcessError);
    
    // Connect state changed signal for UI updates
    connect(m_process, &QProcess::stateChanged,
            this, &MainWindow::onProcessStateChanged);
}

// ============================================================================
// KEY DESIGN PRINCIPLES DEMONSTRATED
// ============================================================================

/*
 * 1. NO MODIFICATIONS TO foot-Function
 *    - All integration via QProcess and temporary wrapper script
 *    - foot-Function code remains completely untouched
 * 
 * 2. COMPREHENSIVE ERROR HANDLING
 *    - Input validation before process start
 *    - Process error detection and reporting
 *    - User-friendly error messages
 * 
 * 3. REAL-TIME OUTPUT CAPTURE
 *    - Stdout captured and displayed in black
 *    - Stderr captured and displayed in red
 *    - Auto-scroll for continuous monitoring
 * 
 * 4. PROCESS CONTROL
 *    - Start process with custom arguments
 *    - Stop process gracefully (or force kill)
 *    - Process state tracking
 * 
 * 5. USER-FRIENDLY INTERFACE
 *    - File dialogs for easy selection
 *    - Status bar updates
 *    - Success/error dialogs
 *    - Controls disabled during processing
 * 
 * 6. ARGUMENT PASSING
 *    - Input video path from UI → Python script
 *    - Model file path from UI → Python script
 *    - Output directory from UI → Python script
 *    - Use stubs flag from UI → Python script
 */
