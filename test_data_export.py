#!/usr/bin/env python
"""
Test script for data export functionality
"""

import os
import json
import csv
import tempfile
import numpy as np
from utils.data_export import export_analysis_data

def create_sample_tracks():
    """Create sample tracking data for testing"""
    tracks = {
        'players': [],
        'ball': [],
        'referees': []
    }
    
    # Create sample data for 100 frames
    for frame in range(100):
        # Player data
        player_frame = {
            1: {
                'bbox': [100, 200, 150, 300],
                'position': [125, 300],
                'team': 1,
                'team_color': (255, 0, 0),
                'speed': 15.5 + frame * 0.1,
                'distance': 10.0 + frame * 0.5,
                'has_ball': frame % 20 == 0
            },
            2: {
                'bbox': [300, 200, 350, 300],
                'position': [325, 300],
                'team': 2,
                'team_color': (0, 0, 255),
                'speed': 12.3 + frame * 0.08,
                'distance': 8.0 + frame * 0.4,
                'has_ball': False
            }
        }
        tracks['players'].append(player_frame)
        
        # Ball data
        ball_frame = {
            1: {
                'bbox': [200, 250, 220, 270],
                'position': [210, 260]
            }
        }
        tracks['ball'].append(ball_frame)
        
        # Referee data (empty for this test)
        tracks['referees'].append({})
    
    return tracks

def create_sample_team_ball_control():
    """Create sample team ball control data"""
    # Alternate between team 1 and team 2
    control = []
    for i in range(100):
        if i < 40:
            control.append(1)
        elif i < 70:
            control.append(2)
        else:
            control.append(1)
    return np.array(control)

def create_sample_camera_movement():
    """Create sample camera movement data"""
    movement = []
    for i in range(100):
        movement.append([i * 0.5, i * 0.3])
    return movement

def test_export_functionality():
    """Test the export functionality"""
    print("Testing data export functionality...")
    
    # Create sample data
    tracks = create_sample_tracks()
    team_ball_control = create_sample_team_ball_control()
    camera_movement = create_sample_camera_movement()
    
    # Export data - use cross-platform temp directory
    output_dir = os.path.join(tempfile.gettempdir(), 'test_export')
    os.makedirs(output_dir, exist_ok=True)
    
    print(f"\nExporting to {output_dir}...")
    exported_files = export_analysis_data(
        tracks=tracks,
        team_ball_control=team_ball_control,
        camera_movement_per_frame=camera_movement,
        output_dir=output_dir,
        filename_prefix='test_analysis'
    )
    
    print("\nExported files:")
    for file_type, file_path in exported_files.items():
        print(f"  - {file_type}: {file_path}")
        if os.path.exists(file_path):
            file_size = os.path.getsize(file_path)
            print(f"    Size: {file_size} bytes")
        else:
            print(f"    ERROR: File not found!")
    
    # Verify JSON files can be read
    print("\nVerifying JSON files...")
    for file_type in ['ball_tracking', 'team_possession', 'summary']:
        if file_type in exported_files:
            file_path = exported_files[file_type]
            try:
                with open(file_path, 'r') as f:
                    data = json.load(f)
                print(f"  ✓ {file_type}: Valid JSON with {len(data) if isinstance(data, list) else len(data.keys())} entries")
            except Exception as e:
                print(f"  ✗ {file_type}: ERROR - {e}")
    
    # Verify CSV files can be read
    print("\nVerifying CSV files...")
    for file_type in ['player_stats', 'camera_movement']:
        if file_type in exported_files:
            file_path = exported_files[file_type]
            try:
                with open(file_path, 'r') as f:
                    reader = csv.DictReader(f)
                    rows = list(reader)
                print(f"  ✓ {file_type}: Valid CSV with {len(rows)} rows")
                if len(rows) > 0:
                    print(f"    Columns: {', '.join(rows[0].keys())}")
            except Exception as e:
                print(f"  ✗ {file_type}: ERROR - {e}")
    
    # Check team possession statistics
    print("\nChecking team possession statistics...")
    if 'team_possession' in exported_files:
        with open(exported_files['team_possession'], 'r') as f:
            possession_data = json.load(f)
        print(f"  Team 1: {possession_data['team_1_percentage']}%")
        print(f"  Team 2: {possession_data['team_2_percentage']}%")
        print(f"  Total frames: {possession_data['total_frames']}")
    
    # Check summary
    print("\nChecking comprehensive summary...")
    if 'summary' in exported_files:
        with open(exported_files['summary'], 'r') as f:
            summary = json.load(f)
        print(f"  Total frames: {summary['metadata']['total_frames']}")
        print(f"  Players tracked: {len(summary['player_summary'])}")
        print(f"  Ball detections: {summary['ball_summary']['total_detections']}")
        
        # Display player stats
        print("\n  Player Statistics:")
        for player_id, stats in summary['player_summary'].items():
            print(f"    Player {player_id} (Team {stats['team']}): {stats['max_speed_kmh']:.2f} km/h max, {stats['total_distance_m']:.2f}m total")
    
    print("\n✓ All tests passed successfully!")
    return True

if __name__ == '__main__':
    try:
        test_export_functionality()
    except Exception as e:
        print(f"\n✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
