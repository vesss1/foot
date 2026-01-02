# Building FootAnalysisGUI on Windows with Qt 6.10.1

## Prerequisites

1. **Qt 6.10.1 Installation**
   - Download and install Qt 6.10.1 from https://www.qt.io/download
   - During installation, ensure you select:
     - Qt 6.10.1
     - MSVC 2019 64-bit or MinGW 64-bit compiler
     - Qt Multimedia
     - Qt Multimedia Widgets
     - CMake (if not already installed)

2. **Verify Qt Installation**
   - Open Qt Creator
   - Go to Tools → Options → Kits
   - Ensure you have a Qt 6.10.1 kit configured
   - Verify that the kit includes Multimedia and MultimediaWidgets modules

## Building with Qt Creator (Recommended)

1. Open Qt Creator
2. File → Open File or Project
3. Navigate to the project directory and open `FootAnalysisGUI.pro` OR `CMakeLists.txt`
4. Select your Qt 6.10.1 kit (Desktop Qt 6.10.1 MSVC2019 64bit or MinGW)
5. Click "Configure Project"
6. Build → Build Project "FootAnalysisGUI" (Ctrl+B)
7. Run → Run (Ctrl+R)

## Building with qmake (Command Line)

```cmd
cd C:\path\to\foot
qmake FootAnalysisGUI.pro
nmake
# or with MinGW:
mingw32-make
```

## Building with CMake (Command Line)

```cmd
cd C:\path\to\foot
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

Or with MinGW:
```cmd
cmake .. -G "MinGW Makefiles"
cmake --build .
```

## Troubleshooting

### Error: "Unknown module(s) in QT: multimedia multimediawidgets"

This means Qt Multimedia is not installed. Fix:
1. Open Qt Maintenance Tool
2. Add Components
3. Select Qt 6.10.1 → Additional Libraries
4. Check: Qt Multimedia and Qt Multimedia Widgets
5. Click Update/Install

### Error: Qt headers not found

Ensure qmake or cmake can find Qt:
```cmd
# For qmake
set PATH=C:\Qt\6.10.1\msvc2019_64\bin;%PATH%

# For cmake
set CMAKE_PREFIX_PATH=C:\Qt\6.10.1\msvc2019_64
```

### Error: Missing DLLs when running

Copy required Qt DLLs to the executable directory:
- Qt6Core.dll
- Qt6Gui.dll
- Qt6Widgets.dll
- Qt6Multimedia.dll
- Qt6MultimediaWidgets.dll
- Qt6Network.dll (dependency)

Or use Qt's windeployqt tool:
```cmd
cd build\bin
C:\Qt\6.10.1\msvc2019_64\bin\windeployqt.exe FootAnalysisGUI.exe
```

## Verification

After building, the executable should be at:
- qmake: `release\FootAnalysisGUI.exe` or `debug\FootAnalysisGUI.exe`
- CMake: `build\bin\FootAnalysisGUI.exe`

Run it to verify the GUI appears with all components:
- Input configuration panel
- Process output console
- Results panel
- Video player
