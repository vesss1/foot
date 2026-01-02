cmake_minimum_required(VERSION 3.16)

project(FootAnalysisGUI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(FootAnalysisGUI
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

target_link_libraries(FootAnalysisGUI PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(WIN32)
    set_property(TARGET FootAnalysisGUI PROPERTY WIN32_EXECUTABLE TRUE)
endif()
