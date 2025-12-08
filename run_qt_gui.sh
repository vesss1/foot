#!/bin/bash
# Launcher script for Football Analysis Qt GUI

echo "==================================="
echo "Football Analysis Qt GUI Launcher"
echo "==================================="
echo ""

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 is not installed or not in PATH"
    echo "Please install Python 3.7 or higher"
    exit 1
fi

echo "Python version: $(python3 --version)"
echo ""

# Check if PyQt5 is installed
if ! python3 -c "import PyQt5" 2>/dev/null; then
    echo "WARNING: PyQt5 is not installed"
    echo "Installing required dependencies..."
    echo ""
    
    if [ -f "requirements.txt" ]; then
        pip install -r requirements.txt
    else
        pip install PyQt5 ultralytics supervision opencv-python numpy matplotlib pandas scikit-learn
    fi
    
    echo ""
fi

# Check if model file exists
if [ ! -f "models/best.pt" ]; then
    echo "WARNING: Model file 'models/best.pt' not found"
    echo "Please download the trained YOLO model and place it in the models/ directory"
    echo "Download from: https://drive.google.com/file/d/1DC2kCygbBWUKheQ_9cFziCsYVSRw6axK/view"
    echo ""
    read -p "Press Enter to continue anyway, or Ctrl+C to exit..."
    echo ""
fi

# Launch the Qt GUI
echo "Launching Football Analysis Qt GUI..."
echo ""
python3 qt_main.py

echo ""
echo "Application closed."
