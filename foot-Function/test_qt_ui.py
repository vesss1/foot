#!/usr/bin/env python3
"""
Test script for Qt UI application
Tests core functionality without requiring GUI display
"""

import sys
import os
import json

# Set QT_QPA_PLATFORM to offscreen for headless testing
os.environ['QT_QPA_PLATFORM'] = 'offscreen'

from PyQt5.QtWidgets import QApplication
from qt_ui_app import FootballAnalysisUI


def test_ui_instantiation():
    """Test that the UI can be instantiated"""
    print("Test 1: UI Instantiation...")
    app = QApplication(sys.argv)
    window = FootballAnalysisUI()
    print("✓ UI created successfully")
    return app, window


def test_data_loading(window):
    """Test that data can be loaded"""
    print("\nTest 2: Data Loading...")
    
    # Check if test data exists
    data_path = 'output_videos/data_output.json'
    if not os.path.exists(data_path):
        print("✗ Test data not found")
        return False
    
    # Load statistics
    window.load_statistics(data_path)
    
    # Verify data was loaded
    if window.stats_data is None:
        print("✗ Failed to load statistics")
        return False
    
    print("✓ Statistics loaded successfully")
    
    # Verify summary data
    summary = window.stats_data.get('summary', {})
    team1_poss = summary.get('team_1_possession_percent', 0)
    team2_poss = summary.get('team_2_possession_percent', 0)
    
    print(f"  Team 1 Possession: {team1_poss}%")
    print(f"  Team 2 Possession: {team2_poss}%")
    
    if team1_poss == 0 and team2_poss == 0:
        print("✗ Invalid possession data")
        return False
    
    print("✓ Data validation passed")
    return True


def test_video_loading(window):
    """Test that video can be loaded"""
    print("\nTest 3: Video Loading...")
    
    video_path = 'output_videos/output_video.avi'
    if not os.path.exists(video_path):
        print("✗ Test video not found")
        return False
    
    window.load_video(video_path)
    
    # Verify video was loaded
    if window.frames is None or len(window.frames) == 0:
        print("✗ Failed to load video frames")
        return False
    
    print(f"✓ Video loaded successfully ({len(window.frames)} frames)")
    print(f"  FPS: {window.fps}")
    print(f"  Total Frames: {window.total_frames}")
    
    return True


def test_statistics_calculation(window):
    """Test that statistics are calculated correctly"""
    print("\nTest 4: Statistics Calculation...")
    
    if window.stats_data is None:
        print("✗ No data loaded")
        return False
    
    # Calculate total distance
    total_distance = 0
    player_count = 0
    
    for key in window.stats_data:
        if key.startswith('team_'):
            team_data = window.stats_data[key]
            if isinstance(team_data, dict):
                for player_id, player_data in team_data.items():
                    if isinstance(player_data, dict):
                        distance_km = player_data.get('distance_km', 0)
                        total_distance += distance_km
                        player_count += 1
    
    print(f"  Total Distance: {total_distance:.2f} km")
    print(f"  Total Players: {player_count}")
    
    if player_count > 0:
        avg_distance = total_distance / player_count
        print(f"  Average Distance per Player: {avg_distance:.2f} km")
    
    if total_distance == 0:
        print("✗ Invalid distance calculation")
        return False
    
    print("✓ Statistics calculation passed")
    return True


def test_ui_labels(window):
    """Test that UI labels are updated correctly"""
    print("\nTest 5: UI Label Updates...")
    
    # Update statistics display
    window.update_statistics_display()
    
    # Check possession labels
    team1_text = window.team1_possession_label.text()
    team2_text = window.team2_possession_label.text()
    
    print(f"  Team 1 Label: {team1_text}")
    print(f"  Team 2 Label: {team2_text}")
    
    if team1_text == "---%":
        print("✗ Team 1 label not updated")
        return False
    
    if team2_text == "---%":
        print("✗ Team 2 label not updated")
        return False
    
    # Check distance labels
    total_dist_text = window.total_distance_label.text()
    avg_dist_text = window.avg_distance_label.text()
    
    print(f"  Total Distance Label: {total_dist_text}")
    print(f"  Average Distance Label: {avg_dist_text}")
    
    if total_dist_text == "--- km":
        print("✗ Total distance label not updated")
        return False
    
    print("✓ UI labels updated correctly")
    return True


def main():
    """
    Run all tests for the Qt UI application.
    
    Tests the following components:
    - UI instantiation and initialization
    - Data loading from JSON files
    - Video loading and frame processing
    - Statistics calculation accuracy
    - UI label updates and display
    
    Returns:
        int: 0 if all tests pass, 1 if any test fails
    
    Requirements:
    - Test data files must exist in output_videos/ directory
    - QT_QPA_PLATFORM environment variable set to 'offscreen'
    - PyQt5 and OpenCV must be installed
    """
    print("="*60)
    print("Qt UI Application Test Suite")
    print("="*60)
    
    try:
        # Test 1: Instantiation
        app, window = test_ui_instantiation()
        
        # Test 2: Data Loading
        if not test_data_loading(window):
            print("\n✗ Data loading test failed")
            return 1
        
        # Test 3: Video Loading
        if not test_video_loading(window):
            print("\n✗ Video loading test failed")
            return 1
        
        # Test 4: Statistics Calculation
        if not test_statistics_calculation(window):
            print("\n✗ Statistics calculation test failed")
            return 1
        
        # Test 5: UI Labels
        if not test_ui_labels(window):
            print("\n✗ UI label test failed")
            return 1
        
        print("\n" + "="*60)
        print("All tests passed! ✓")
        print("="*60)
        
        return 0
        
    except Exception as e:
        print(f"\n✗ Test suite failed with exception: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
