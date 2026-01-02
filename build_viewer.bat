@echo off
REM Video Data Viewer - Windows Build Script
REM This script helps you build the VideoDataViewer application

echo ========================================
echo Video Data Viewer - Build Script
echo ========================================
echo.

REM Check if qmake is available
where qmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: qmake not found in PATH
    echo.
    echo Please ensure Qt is installed and qmake is in your PATH
    echo Example: C:\Qt\6.10.1\msvc2019_64\bin
    echo.
    pause
    exit /b 1
)

echo Found qmake: 
qmake --version
echo.

REM Check if OpenCV is accessible
echo Checking for OpenCV...
if exist "C:\opencv\build\include\opencv2\opencv.hpp" (
    echo Found OpenCV at C:\opencv
) else (
    echo WARNING: OpenCV not found at C:\opencv
    echo Please install OpenCV and update paths in VideoDataViewer.pro
    echo Download from: https://opencv.org/releases/
    echo.
)

echo.
echo ========================================
echo Building VideoDataViewer...
echo ========================================
echo.

REM Clean previous build
if exist Makefile (
    echo Cleaning previous build...
    nmake clean 2>nul
    del Makefile 2>nul
)

REM Run qmake
echo Running qmake...
qmake VideoDataViewer.pro
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: qmake failed
    pause
    exit /b 1
)

REM Build the project
echo.
echo Building project...
nmake
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed
    echo.
    echo If you're using MinGW, try: mingw32-make
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location:
if exist "release\VideoDataViewer.exe" (
    echo   release\VideoDataViewer.exe
) else if exist "debug\VideoDataViewer.exe" (
    echo   debug\VideoDataViewer.exe
) else (
    echo   Build completed but executable location unknown
)
echo.
echo To run: 
echo   1. Ensure output.avi exists in foot-Function\output_videos\
echo   2. Double-click VideoDataViewer.exe or run from command line
echo.
pause
