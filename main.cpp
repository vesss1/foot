#include "MainWindow.h"
#include <QApplication>

/**
 * @brief Main entry point for the Football Analysis Qt GUI
 * 
 * This application provides a graphical user interface to the existing
 * foot-Function Python-based football video analysis pipeline.
 * 
 * The GUI uses QProcess to execute the Python scripts without modifying
 * the original foot-Function code.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("Football Analysis GUI");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Football Analysis");
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}
