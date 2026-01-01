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
    echo "PyQt5 is not installed."
    echo "Would you like to install the required dependencies? (y/n)"
    read -r response
    if [[ "$response" =~ ^[Yy]$ ]]; then
        echo "Installing dependencies..."
        pip3 install --user -r requirements.txt
    else
        echo "Cannot run GUI without PyQt5. Please install dependencies manually:"
        echo "  pip3 install -r requirements.txt"
        exit 1
    fi
fi

# Run the GUI application
python3 gui.py
