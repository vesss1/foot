/*******************************************************************************
 * APPLICATION ENTRY POINT
 * 
 * This is the main entry point for the Football Analysis GUI application.
 * It initializes the Qt application framework and creates the main window.
 * 
 * EXECUTION FLOW:
 * 1. Initialize Qt application with command-line arguments
 * 2. Create MainWindow instance (the primary GUI)
 * 3. Display the window to the user
 * 4. Enter Qt event loop and wait for user interactions
 * 
 * The event loop (app.exec()) processes all GUI events including:
 * - User input (clicks, keyboard)
 * - Window operations (resize, close)
 * - Process signals from Python backend
 * - Timer events for progress updates
 ******************************************************************************/

#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Initialize Qt application framework
    QApplication app(argc, argv);
    
    // Create and show the main application window
    MainWindow window;
    window.show();
    
    // Start the event loop (blocks until application exits)
    return app.exec();
}
