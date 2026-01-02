QT += core gui widgets

CONFIG += c++17

# OpenCV configuration
# Adjust these paths based on your OpenCV installation
# For Windows: typically C:/opencv/build
# For Linux: typically /usr/local or /usr
# For macOS: typically /usr/local (via Homebrew)

# Windows OpenCV configuration
win32 {
    INCLUDEPATH += C:/opencv/build/include
    
    CONFIG(debug, debug|release) {
        LIBS += -LC:/opencv/build/x64/vc16/lib \
                -lopencv_world4101d
    } else {
        LIBS += -LC:/opencv/build/x64/vc16/lib \
                -lopencv_world4101
    }
}

# Linux OpenCV configuration
unix:!macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4
    
    # Fallback if pkg-config doesn't work
    isEmpty(PKGCONFIG) {
        INCLUDEPATH += /usr/local/include/opencv4
        LIBS += -L/usr/local/lib \
                -lopencv_core \
                -lopencv_imgproc \
                -lopencv_highgui \
                -lopencv_videoio \
                -lopencv_imgcodecs
    }
}

# macOS OpenCV configuration
macx {
    INCLUDEPATH += /usr/local/include/opencv4
    LIBS += -L/usr/local/lib \
            -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lopencv_videoio \
            -lopencv_imgcodecs
}

SOURCES += \
    viewer_main.cpp \
    VideoDataViewer.cpp

HEADERS += \
    VideoDataViewer.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TARGET = VideoDataViewer
