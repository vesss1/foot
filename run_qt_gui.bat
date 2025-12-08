@echo off
REM Launcher script for Football Analysis Qt GUI (Windows)

echo ===================================
echo Football Analysis Qt GUI Launcher
echo ===================================
echo.

REM Check if Python is available
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.7 or higher from https://www.python.org/
    pause
    exit /b 1
)

echo Python version:
python --version
echo.

REM Check if PyQt5 is installed
python -c "import PyQt5" >nul 2>&1
if errorlevel 1 (
    echo WARNING: PyQt5 is not installed
    echo Installing required dependencies...
    echo.
    
    if exist requirements.txt (
        pip install -r requirements.txt
    ) else (
        pip install PyQt5 ultralytics supervision opencv-python numpy matplotlib pandas scikit-learn
    )
    echo.
)

REM Check if model file exists
if not exist "models\best.pt" (
    echo WARNING: Model file 'models\best.pt' not found
    echo Please download the trained YOLO model and place it in the models\ directory
    echo Download from: https://drive.google.com/file/d/1DC2kCygbBWUKheQ_9cFziCsYVSRw6axK/view
    echo.
    pause
)

REM Launch the Qt GUI
echo Launching Football Analysis Qt GUI...
echo.
python qt_main.py

echo.
echo Application closed.
pause
