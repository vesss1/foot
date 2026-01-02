#include "VideoDataViewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    VideoDataViewer viewer;
    viewer.show();
    
    return app.exec();
}
