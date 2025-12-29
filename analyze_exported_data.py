#!/usr/bin/env python
"""
Example script showing how to use exported analysis data

This script demonstrates how to load and analyze the exported JSON and CSV files
from the football analysis system.
"""

import json
import pandas as pd
import os
from glob import glob

def analyze_exported_data(export_dir='output_videos'):
    """
    Load and analyze exported data files
    
    Args:
        export_dir: Directory containing exported files
    """
    print(f"Looking for exported data in {export_dir}/\n")
    
    # Find the most recent summary file (with any prefix)
    summary_files = glob(os.path.join(export_dir, '*_summary_*.json'))
    if not summary_files:
        print("No summary files found. Please run the analysis first.")
        return
    
    # Use the most recent file
    summary_file = max(summary_files, key=os.path.getctime)
    print(f"Loading summary from: {os.path.basename(summary_file)}\n")
    
    # Load summary
    with open(summary_file, 'r', encoding='utf-8') as f:
        summary = json.load(f)
    
    # Display metadata
    print("=== Analysis Metadata ===")
    print(f"Export timestamp: {summary['metadata']['export_timestamp']}")
    print(f"Total frames: {summary['metadata']['total_frames']}")
    print()
    
    # Display team possession
    print("=== Team Possession ===")
    team1 = summary['team_possession']['team_1']
    team2 = summary['team_possession']['team_2']
    print(f"Team 1: {team1['percentage']:.1f}% ({team1['frames']} frames)")
    print(f"Team 2: {team2['percentage']:.1f}% ({team2['frames']} frames)")
    print()
    
    # Display player statistics
    print("=== Player Statistics ===")
    print(f"{'Player ID':<12} {'Team':<6} {'Max Speed (km/h)':<18} {'Distance (m)':<15} {'Ball Possession'}")
    print("-" * 80)
    
    for player_id, stats in summary['player_summary'].items():
        possession_pct = (stats['frames_with_ball'] / stats['total_frames'] * 100) if stats['total_frames'] > 0 else 0
        print(f"{player_id:<12} {stats['team']:<6} {stats['max_speed_kmh']:<18.2f} {stats['total_distance_m']:<15.2f} {possession_pct:.1f}%")
    print()
    
    # Display ball detection stats
    print("=== Ball Detection ===")
    ball_summary = summary['ball_summary']
    print(f"Total detections: {ball_summary['total_detections']}")
    print(f"Detection rate: {ball_summary['detection_rate']:.1f}%")
    print()
    
    # Display camera movement stats
    print("=== Camera Movement ===")
    cam_summary = summary['camera_movement_summary']
    print(f"Total frames: {cam_summary['total_frames']}")
    print(f"Total X movement: {cam_summary['total_movement_x']:.2f} pixels")
    print(f"Total Y movement: {cam_summary['total_movement_y']:.2f} pixels")
    print()
    
    # Load and analyze player stats CSV
    player_stats_files = glob(os.path.join(export_dir, '*_player_stats_*.csv'))
    if player_stats_files:
        player_stats_file = max(player_stats_files, key=os.path.getctime)
        print(f"Loading detailed player stats from: {os.path.basename(player_stats_file)}\n")
        
        df = pd.read_csv(player_stats_file)
        
        print("=== Detailed Player Analysis (from CSV) ===")
        
        # Average speed per player
        print("\nAverage Speed per Player:")
        avg_speeds = df[df['speed_kmh'] > 0].groupby('player_id')['speed_kmh'].mean()
        for player_id, avg_speed in avg_speeds.items():
            print(f"  Player {player_id}: {avg_speed:.2f} km/h")
        
        # Frames with ball per player
        print("\nFrames with Ball:")
        ball_frames = df[df['has_ball'] == True].groupby('player_id').size()
        for player_id, count in ball_frames.items():
            print(f"  Player {player_id}: {count} frames")
        
        # Maximum distance covered
        print("\nMaximum Distance Covered:")
        max_distances = df.groupby('player_id')['distance_m'].max()
        for player_id, max_dist in max_distances.items():
            print(f"  Player {player_id}: {max_dist:.2f} meters")
    
    # Load team possession data
    possession_files = glob(os.path.join(export_dir, '*_team_possession_*.json'))
    if possession_files:
        possession_file = max(possession_files, key=os.path.getctime)
        with open(possession_file, 'r', encoding='utf-8') as f:
            possession_data = json.load(f)
        
        print("\n=== Possession Trends ===")
        # Calculate possession streaks
        possession_by_frame = possession_data['possession_by_frame']
        
        # Find longest possession streak for each team
        team1_streaks = []
        team2_streaks = []
        current_streak = 1
        current_team = possession_by_frame[0]
        
        for i in range(1, len(possession_by_frame)):
            if possession_by_frame[i] == current_team:
                current_streak += 1
            else:
                if current_team == 1:
                    team1_streaks.append(current_streak)
                else:
                    team2_streaks.append(current_streak)
                current_team = possession_by_frame[i]
                current_streak = 1
        
        # Add the last streak
        if current_team == 1:
            team1_streaks.append(current_streak)
        else:
            team2_streaks.append(current_streak)
        
        if team1_streaks:
            print(f"Team 1 longest possession: {max(team1_streaks)} frames")
        if team2_streaks:
            print(f"Team 2 longest possession: {max(team2_streaks)} frames")
    
    print("\n" + "=" * 80)
    print("Analysis complete!")

if __name__ == '__main__':
    import sys
    
    # Allow specifying a custom export directory
    export_dir = sys.argv[1] if len(sys.argv) > 1 else 'output_videos'
    
    try:
        analyze_exported_data(export_dir)
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
