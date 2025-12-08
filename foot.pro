# Football Analysis Project - Qt Creator Project File
# This file allows opening the Python/PyQt5 project in Qt Creator IDE

TEMPLATE = app
TARGET = FootballAnalysis

# This is a Python project, not C++
CONFIG -= qt
CONFIG += console

# Define source files (Python files)
SOURCES += \
    qt_main.py \
    main.py \
    yolo_inference.py \
    camera_movement_estimator/__init__.py \
    camera_movement_estimator/camera_movement_estimator.py \
    player_ball_assigner/__init__.py \
    player_ball_assigner/player_ball_assigner.py \
    speed_and_distance_estimator/__init__.py \
    speed_and_distance_estimator/speed_and_distance_estimator.py \
    team_assigner/__init__.py \
    team_assigner/team_assigner.py \
    trackers/__init__.py \
    trackers/tracker.py \
    utils/__init__.py \
    utils/bbox_utils.py \
    utils/video_utils.py \
    view_transformer/__init__.py \
    view_transformer/view_transformer.py

# Documentation and other files
OTHER_FILES += \
    README.md \
    QT_GUI_GUIDE.md \
    requirements.txt \
    .gitignore \
    run_qt_gui.sh \
    run_qt_gui.bat

# Define directories
INCLUDEPATH += . \
    camera_movement_estimator \
    player_ball_assigner \
    speed_and_distance_estimator \
    team_assigner \
    trackers \
    utils \
    view_transformer

# Data directories (for reference only)
DISTFILES += \
    models/* \
    input_videos/* \
    output_videos/* \
    stubs/*
