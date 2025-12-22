# Qt Creator Integration Setup Guide

## Overview

This guide provides detailed instructions on how to integrate the Football Analysis project with Qt Creator IDE. Although this is a Python/PyQt5 project rather than a C++ project, Qt Creator provides excellent project management, code navigation, and editing capabilities, making it an ideal choice for developing and maintaining this project.

## Why Use Qt Creator?

### Advantages

1. **Project Structure Visualization**
   - Clear file tree structure
   - Organized source files by module
   - Quick access to all project files

2. **Powerful Code Editing Features**
   - Syntax highlighting (Python support)
   - Code auto-completion
   - Fast search and navigation
   - Multi-file editing

3. **Integrated Terminal**
   - Run Python scripts directly within the IDE
   - No need to switch windows
   - Easy output and error monitoring

4. **Version Control Integration**
   - Built-in Git support
   - Visual diff comparison
   - Easy commit and push changes

5. **Cross-platform Support**
   - Consistent experience on Windows, macOS, and Linux
   - Cross-platform project management

## System Requirements

### Required Software

1. **Qt Creator**
   - Version: 6.0 or higher (latest version recommended)
   - Download: [Qt Creator Official Website](https://www.qt.io/product/development-tools)
   - Note: You can install Qt Creator standalone without the full Qt SDK

2. **Python**
   - Version: 3.7 or higher
   - Including pip package manager

3. **Project Dependencies**
   - See `requirements.txt`
   - Install command: `pip install -r requirements.txt`

## Installing Qt Creator

### Windows

1. Download the Qt online installer from [Qt Official Website](https://www.qt.io/download-qt-installer)
2. Run the installer
3. In component selection, choose at least:
   - Qt Creator (under Developer and Designer Tools)
4. Complete the installation

### macOS

```bash
# Install using Homebrew
brew install --cask qt-creator

# Or download the .dmg file from the official website
```

### Linux (Ubuntu/Debian)

```bash
# Install from package manager
sudo apt update
sudo apt install qtcreator

# Or use snap
sudo snap install qtcreator --classic
```

### Linux (Fedora/RHEL)

```bash
sudo dnf install qt-creator
```

## Opening the Project in Qt Creator

### Method 1: Open from Qt Creator

1. Launch Qt Creator
2. Click menu: **File** → **Open File or Project...**
3. Navigate to the project directory
4. Select the `foot.pro` file
5. Click **Open**

### Method 2: Open from File Manager

1. Navigate to the project directory in your file manager
2. Double-click the `foot.pro` file
3. The system should automatically open it with Qt Creator

### Method 3: Open from Command Line

```bash
# Navigate to the project directory
cd /path/to/foot

# Open the project with Qt Creator
qtcreator foot.pro
```

## Project Configuration

### First Time Opening

When you first open the project in Qt Creator, you may see the project configuration screen:

1. **Configure Project**: Click this button to accept the default settings
2. Qt Creator will create a `foot.pro.user` file (this file is in `.gitignore` and won't be committed to version control)

### Project Structure

Once opened, you'll see in the **Projects** panel on the left:

```
FootballAnalysis
├── Sources
│   ├── qt_main.py              # Qt GUI main program
│   ├── main.py                 # Command-line main program
│   ├── yolo_inference.py       # YOLO inference
│   ├── camera_movement_estimator/
│   │   ├── __init__.py
│   │   └── camera_movement_estimator.py
│   ├── player_ball_assigner/
│   │   ├── __init__.py
│   │   └── player_ball_assigner.py
│   ├── speed_and_distance_estimator/
│   │   ├── __init__.py
│   │   └── speed_and_distance_estimator.py
│   ├── team_assigner/
│   │   ├── __init__.py
│   │   └── team_assigner.py
│   ├── trackers/
│   │   ├── __init__.py
│   │   └── tracker.py
│   ├── utils/
│   │   ├── __init__.py
│   │   ├── bbox_utils.py
│   │   └── video_utils.py
│   └── view_transformer/
│       ├── __init__.py
│       └── view_transformer.py
│
└── Other Files
    ├── README.md               # Project description (Chinese)
    ├── README_EN.md            # Project description (English)
    ├── README_ZH.md            # Project description (Traditional Chinese)
    ├── USAGE_ZH.md             # Usage guide
    ├── QT_GUI_GUIDE.md         # Qt GUI guide (English)
    ├── QT_GUI_GUIDE_ZH.md      # Qt GUI guide (Chinese)
    ├── 中文文檔.md              # Chinese documentation
    ├── requirements.txt         # Python dependencies
    ├── .gitignore              # Git ignore file
    ├── run_qt_gui.sh           # Linux/Mac run script
    └── run_qt_gui.bat          # Windows run script
```

## Working in Qt Creator

### Browsing Code

1. **File Tree Structure**
   - Use the **Projects** panel on the left to browse files
   - Double-click a file to open it in the editor

2. **Quick Navigation**
   - **Ctrl+K** (Windows/Linux) or **Cmd+K** (macOS): Quick open file
   - **Ctrl+Shift+F**: Search text in project
   - **F2**: Jump to definition (limited Python support)

3. **Split View**
   - Right-click on editor tab
   - Select **Split** to view multiple files side by side

### Editing Code

1. **Syntax Highlighting**
   - Qt Creator automatically recognizes Python files and provides syntax highlighting

2. **Indentation and Formatting**
   - Qt Creator follows Python indentation rules
   - Use Tab or spaces (based on project settings)

3. **Comments**
   - **Ctrl+/** (Windows/Linux) or **Cmd+/** (macOS): Toggle line comment

### Running the Project

Qt Creator is primarily used for code editing and project management. To run the Python application:

#### Option 1: Using Integrated Terminal

1. Click menu: **Tools** → **External** → **Text** → **Open Terminal**
   - Or press **Alt+T**, then **Alt+Shift+T**

2. Run in terminal:
   ```bash
   # Run Qt GUI application
   python qt_main.py
   
   # Or run command-line version
   python main.py
   ```

#### Option 2: Using External Terminal

1. Open your system terminal (Terminal, Command Prompt, PowerShell)
2. Navigate to the project directory
3. Run Python scripts

#### Option 3: Configure Custom Run Configuration

You can configure a custom run configuration in Qt Creator:

1. Click menu: **Projects** → **Run**
2. Click **Add** → **Custom Executable**
3. Configure:
   - **Executable**: `python` or `python3`
   - **Command line arguments**: `qt_main.py`
   - **Working directory**: `%{sourceDir}`
4. Click **Apply**

Now you can press **Ctrl+R** (Windows/Linux) or **Cmd+R** (macOS) to run the application.

### Debugging (Limited Support)

Qt Creator primarily supports C++ debugging, but you can:

1. Use Python debugger (pdb) to add breakpoints in code:
   ```python
   import pdb; pdb.set_trace()
   ```

2. Or use specialized Python IDEs like Visual Studio Code for debugging

### Version Control

Qt Creator has built-in Git support:

1. **View Changes**
   - Click menu: **Tools** → **Git** → **Diff**

2. **Commit Changes**
   - Click menu: **Tools** → **Git** → **Local Repository** → **Commit**

3. **Push/Pull**
   - Use corresponding options in Git menu

4. **View History**
   - Click menu: **Tools** → **Git** → **Log**

## Useful Qt Creator Features

### 1. Search and Replace

- **Ctrl+F** (Windows/Linux) or **Cmd+F** (macOS): Search in current file
- **Ctrl+Shift+F**: Search in entire project
- **Ctrl+H** (Windows/Linux) or **Cmd+H** (macOS): Replace in current file

### 2. Bookmarks

- **Ctrl+M**: Toggle bookmark
- **Ctrl+.**: Jump to next bookmark
- **Ctrl+,**: Jump to previous bookmark

### 3. Code Snippets

Qt Creator supports custom code snippets:

1. Click menu: **Tools** → **Options**
2. Select **Text Editor** → **Snippets**
3. Add Python code snippets

### 4. Appearance Customization

1. Click menu: **Tools** → **Options**
2. **Environment** → **Interface**: Change theme
3. **Text Editor** → **Fonts & Colors**: Customize editor appearance

## Frequently Asked Questions

### Q: Can Qt Creator run Python code?

A: Qt Creator is primarily a code editor and project management tool. It can run Python code through the integrated terminal or custom run configurations, but doesn't provide full Python IDE features like PyCharm or VS Code.

### Q: Why choose Qt Creator over other Python IDEs?

A:
- If you're developing PyQt5 applications, Qt Creator provides a familiar environment
- Excellent project structure visualization
- Lightweight and fast
- Cross-platform consistency
- Convenient for developers already familiar with Qt Creator

### Q: Can I use Qt Designer to design the GUI?

A: Yes! Although this project uses code-based GUI creation (in `qt_main.py`), you can:
1. Use Qt Designer to create `.ui` files
2. Convert to Python code using `pyuic5`
3. Integrate into the project

```bash
# Convert .ui file to Python
pyuic5 -x your_design.ui -o ui_your_design.py
```

### Q: What is the foot.pro.user file?

A: The `foot.pro.user` file stores your personal Qt Creator settings (window layout, run configurations, etc.). It's in `.gitignore` and won't be tracked by version control, as these settings are user-specific.

### Q: Can I use Qt Creator's C++ features?

A: This project is a pure Python project, so most C++ specific features don't apply. However, Qt Creator's editor, search, and project management features work perfectly for Python code.

### Q: How do I update the foot.pro file?

A: The `foot.pro` file is a text file listing all files in the project. If you add or remove Python files or documents, edit `foot.pro` and update the `SOURCES` or `OTHER_FILES` sections.

### Q: Does Qt Creator auto-complete Python code?

A: Qt Creator provides basic Python syntax highlighting but doesn't provide advanced auto-completion like PyCharm or VS Code (with Pylance). For advanced Python development features, you may want to consider using a specialized Python IDE.

### Q: Can I use Qt Creator alongside other IDEs?

A: Absolutely! Many developers use:
- **Qt Creator**: For project overview, file management, and quick edits
- **PyCharm/VS Code**: For in-depth Python development and debugging
- **Jupyter Notebook**: For experimentation and prototyping

## Best Practices

### 1. Organize Your Workspace

- Use Qt Creator's project view to browse file structure
- Use bookmarks to mark important code sections
- Utilize split view to see related files simultaneously

### 2. Leverage Integrated Terminal

- Run Python scripts directly within Qt Creator
- Quick test code changes
- Monitor application output

### 3. Use Version Control

- Commit changes regularly
- Write meaningful commit messages
- Use Qt Creator's Git integration to view diffs

### 4. Keep foot.pro Updated

- Update `foot.pro` when adding new files
- Keep project structure clean and organized
- Document important project directories

### 5. Customize Your Environment

- Adjust editor fonts and colors to your preference
- Set up useful keyboard shortcuts
- Configure custom run configurations for efficiency

## Alternatives

While Qt Creator is a great choice, here are other IDEs to consider:

### PyCharm
- **Pros**: Full-featured Python IDE, excellent debugging, intelligent auto-completion
- **Cons**: More heavyweight than Qt Creator
- **Best for**: Professional Python development

### Visual Studio Code
- **Pros**: Lightweight, excellent Python extensions, extensive customization
- **Cons**: Requires extension configuration
- **Best for**: Multi-language developers

### Spyder
- **Pros**: MATLAB-like interface, good for scientific computing
- **Cons**: Primarily for data science, not a general IDE
- **Best for**: Data science and analysis

### Jupyter Notebook/Lab
- **Pros**: Interactive development, great for experimentation
- **Cons**: Not suitable for full application development
- **Best for**: Prototyping and exploratory analysis

## Additional Resources

### Learning Qt Creator

- [Qt Creator Official Documentation](https://doc.qt.io/qtcreator/)
- [Qt Creator Keyboard Shortcuts](https://doc.qt.io/qtcreator/creator-keyboard-shortcuts.html)
- [Qt Creator Video Tutorials](https://www.qt.io/qt-training/)

### Learning PyQt5

- [PyQt5 Official Documentation](https://www.riverbankcomputing.com/static/Docs/PyQt5/)
- [Qt for Python (PySide6) Documentation](https://doc.qt.io/qtforpython/)
- Refer to `qt_main.py` in this project as a practical example

### Project-Specific Documentation

- **[QT_GUI_GUIDE.md](QT_GUI_GUIDE.md)**: Qt GUI application usage guide
- **[README_EN.md](README_EN.md)**: Project overview and features
- **[README.md](README.md)**: Chinese project documentation

## Troubleshooting

### Qt Creator Can't Open foot.pro

**Solution**:
1. Ensure Qt Creator is properly installed
2. Check that `foot.pro` file exists and is not corrupted
3. Try using menu **File → Open File or Project** instead of double-clicking

### Files Not Showing in Project Tree

**Solution**:
1. Check if files are listed in `foot.pro`
2. Add missing files to `SOURCES` or `OTHER_FILES`
3. Close and reopen project to refresh

### Can't Run Python from Qt Creator

**Solution**:
1. Ensure Python is installed and in system PATH
2. Use integrated terminal instead of run configuration
3. Check Python executable path is correct

### Chinese Characters Display Incorrectly

**Solution**:
1. Ensure files are saved with UTF-8 encoding
2. Set in Qt Creator: **Tools → Options → Text Editor → Behavior → File Encodings** to UTF-8
3. Reload files

## Summary

Using Qt Creator for developing this Football Analysis project provides:

✅ Clear project structure visualization  
✅ Powerful code editing and navigation features  
✅ Integrated terminal for running Python  
✅ Built-in version control support  
✅ Cross-platform development environment  
✅ Familiar tooling for PyQt5 development  

While Qt Creator may not provide all advanced features like specialized Python IDEs, it offers an excellent lightweight development environment for this project, especially suitable for developers already familiar with the Qt ecosystem.

---

**Happy developing with Qt Creator!** 🎯🖥️⚽

If you have questions or need assistance, please refer to other documentation in the project or raise an issue on GitHub.
