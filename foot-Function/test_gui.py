#!/usr/bin/env python3
"""
Test script for Football Analysis Qt GUI
Tests the GUI components without requiring a display.
"""

import sys
import os

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_imports():
    """Test that all required modules can be imported."""
    print("Testing imports...")
    
    try:
        import PyQt5
        print("✓ PyQt5 available")
    except ImportError:
        print("✗ PyQt5 not installed - install with: pip install PyQt5")
        return False
    
    try:
        from PyQt5.QtWidgets import QApplication
        print("✓ PyQt5.QtWidgets available")
    except ImportError:
        print("✗ PyQt5.QtWidgets not available")
        return False
    
    try:
        from gui import FootballAnalysisGUI, AnalysisThread
        print("✓ GUI module imported successfully")
        print(f"  - FootballAnalysisGUI class: {FootballAnalysisGUI.__name__}")
        print(f"  - AnalysisThread class: {AnalysisThread.__name__}")
    except ImportError as e:
        print(f"✗ Failed to import GUI module: {e}")
        return False
    
    return True

def test_gui_structure():
    """Test the GUI class structure and methods."""
    print("\nTesting GUI structure...")
    
    from gui import FootballAnalysisGUI
    
    # Check required methods exist
    required_methods = [
        'init_ui',
        'create_config_tab',
        'create_progress_tab', 
        'create_results_tab',
        'browse_input_video',
        'browse_model',
        'browse_output_dir',
        'start_analysis',
        'stop_analysis',
        'update_progress',
        'handle_error',
        'handle_completion',
        'load_results'
    ]
    
    for method in required_methods:
        if hasattr(FootballAnalysisGUI, method):
            print(f"✓ Method '{method}' exists")
        else:
            print(f"✗ Method '{method}' missing")
            return False
    
    return True

def test_thread_structure():
    """Test the AnalysisThread class structure."""
    print("\nTesting AnalysisThread structure...")
    
    from gui import AnalysisThread
    from PyQt5.QtCore import QThread
    
    # Check it's a QThread subclass
    if not issubclass(AnalysisThread, QThread):
        print("✗ AnalysisThread is not a QThread subclass")
        return False
    print("✓ AnalysisThread is a QThread subclass")
    
    # Check required methods
    required_methods = ['run', 'stop']
    
    for method in required_methods:
        if hasattr(AnalysisThread, method):
            print(f"✓ Method '{method}' exists")
        else:
            print(f"✗ Method '{method}' missing")
            return False
    
    # Check signals
    required_signals = ['progress_update', 'error_occurred', 'analysis_complete']
    
    for signal in required_signals:
        if hasattr(AnalysisThread, signal):
            print(f"✓ Signal '{signal}' exists")
        else:
            print(f"✗ Signal '{signal}' missing")
            return False
    
    return True

def test_file_structure():
    """Test that all required files exist."""
    print("\nTesting file structure...")
    
    required_files = [
        'gui.py',
        'main.py',
        'requirements.txt',
        'run_gui.sh',
        'run_gui.bat'
    ]
    
    for filename in required_files:
        if os.path.exists(filename):
            print(f"✓ File '{filename}' exists")
        else:
            print(f"✗ File '{filename}' missing")
            return False
    
    return True

def test_requirements():
    """Test that requirements.txt contains necessary packages."""
    print("\nTesting requirements.txt...")
    
    required_packages = [
        'PyQt5',
        'opencv-python',
        'numpy',
        'ultralytics'
    ]
    
    try:
        with open('requirements.txt', 'r') as f:
            content = f.read()
        
        for package in required_packages:
            if package in content:
                print(f"✓ Package '{package}' in requirements.txt")
            else:
                print(f"✗ Package '{package}' missing from requirements.txt")
                return False
        
        return True
    except Exception as e:
        print(f"✗ Error reading requirements.txt: {e}")
        return False

def test_launcher_scripts():
    """Test that launcher scripts are executable."""
    print("\nTesting launcher scripts...")
    
    # Check Linux/Mac script
    if os.path.exists('run_gui.sh'):
        if os.access('run_gui.sh', os.X_OK):
            print("✓ run_gui.sh is executable")
        else:
            print("⚠ run_gui.sh exists but is not executable (may need: chmod +x run_gui.sh)")
    
    # Check Windows script
    if os.path.exists('run_gui.bat'):
        print("✓ run_gui.bat exists")
    
    return True

def main():
    """Run all tests."""
    print("="*60)
    print("Football Analysis GUI - Test Suite")
    print("="*60)
    
    tests = [
        ("Import Test", test_imports),
        ("File Structure Test", test_file_structure),
        ("Requirements Test", test_requirements),
        ("GUI Structure Test", test_gui_structure),
        ("Thread Structure Test", test_thread_structure),
        ("Launcher Scripts Test", test_launcher_scripts)
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print('='*60)
        try:
            result = test_func()
            results.append((test_name, result))
        except Exception as e:
            print(f"✗ Test failed with exception: {e}")
            import traceback
            traceback.print_exc()
            results.append((test_name, False))
    
    # Summary
    print("\n" + "="*60)
    print("Test Summary")
    print("="*60)
    
    passed = sum(1 for _, result in results if result)
    total = len(results)
    
    for test_name, result in results:
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"{status}: {test_name}")
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n✓ All tests passed! GUI is ready to use.")
        return 0
    else:
        print(f"\n✗ {total - passed} test(s) failed.")
        return 1

if __name__ == '__main__':
    sys.exit(main())
