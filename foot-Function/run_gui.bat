@echo off
REM Launcher script for Football Analysis Qt GUI (Windows)

cd /d "%~dp0"

REM Check if Python is available
python --version >nul 2>&1
if errorlevel 1 (
    echo Error: Python is not installed or not in PATH
    pause
    exit /b 1
)

REM Check if PyQt5 is installed
python -c "import PyQt5" 2>nul
if errorlevel 1 (
    echo PyQt5 is not installed.
    echo Would you like to install the required dependencies? (Y/N)
    set /p response=
    if /i "%response%"=="Y" (
        echo Installing dependencies...
        pip install --user -r requirements.txt
    ) else (
        echo Cannot run GUI without PyQt5. Please install dependencies manually:
        echo   pip install -r requirements.txt
        pause
        exit /b 1
    )
)

REM Run the GUI application
python gui.py

pause
