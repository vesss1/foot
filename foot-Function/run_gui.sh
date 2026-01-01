#!/bin/bash
# Launcher script for Football Analysis Qt GUI

# Change to the foot-Function directory
cd "$(dirname "$0")"

# Check if Python 3 is available
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 is not installed"
    exit 1
fi

# Check if PyQt5 is installed
if ! python3 -c "import PyQt5" 2>/dev/null; then
    echo "PyQt5 is not installed. Installing dependencies..."
    pip3 install -r requirements.txt
fi

# Run the GUI application
python3 gui.py
