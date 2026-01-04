"""
===============================================================================
DATA OUTPUT UTILITIES
===============================================================================

This module handles exporting analysis results to structured data formats
(JSON and CSV) for easy consumption by the GUI and external tools.

OUTPUT STRUCTURE:
JSON and CSV files contain:
- Player statistics (distance traveled per player)
- Team organization (players grouped by team)
- Possession statistics (percentage of ball control per team)

DATA FORMATS:
1. JSON: Hierarchical structure with summary and per-team player data
2. CSV: Tabular format with columns: Team, Player ID, Distance

FILES GENERATED:
- data_output.json: Complete analysis data in JSON format
- data_output.csv: Simplified table for spreadsheet applications

USAGE:
Called at the end of the analysis pipeline to save all computed metrics.
The GUI automatically loads these files to display results in the interface.
===============================================================================
"""

import json
import csv
import os
import numpy as np


def output_data(tracks, output_path='output_videos/data_output.json', team_ball_control=None):
    """
    Output numerical data from video analysis including:
    - Distance covered by each player (in kilometers)
    - Data separated by team
    - Ball possession percentage per team
    
    Args:
        tracks: Dictionary containing tracking information for players
        output_path: Path to save the output data (default: 'output_videos/data_output.json')
        team_ball_control: numpy array with team possession per frame (optional)
    
    Returns:
        Dictionary containing the organized data
    """
    # Initialize data structure with summary only
    output_dict = {
        'summary': {}
    }
    
    # Calculate ball possession percentages if provided
    if team_ball_control is not None:
        # Count possession for each team
        unique_teams, counts = np.unique(team_ball_control, return_counts=True)
        
        # Build possession counts dictionary (excluding 0 which represents no possession)
        possession_counts = {}
        for team_id, count in zip(unique_teams, counts):
            if team_id > 0:
                possession_counts[int(team_id)] = int(count)
        
        # Calculate total possession (sum of all teams)
        total_possession = sum(possession_counts.values())
        
        # Calculate possession percentages
        if total_possession > 0:
            for team_id, count in possession_counts.items():
                percent = (count / total_possession) * 100
                output_dict['summary'][f'team_{team_id}_possession_percent'] = round(percent, 2)
    
    # Process player tracks
    if 'players' in tracks and len(tracks['players']) > 0:
        # Get the last frame to get final distance values
        last_frame = len(tracks['players']) - 1
        
        # Ensure the last frame has data
        if tracks['players'][last_frame]:
            for player_id, player_data in tracks['players'][last_frame].items():
                # Get player team (default to 1 if not available, which should not happen in normal flow)
                # The main pipeline always assigns team 1 or 2
                team = player_data.get('team', 1)
                
                # Get distance in meters and convert to kilometers
                distance_m = player_data.get('distance', 0)
                distance_km = distance_m / 1000
                
                # Organize by team
                team_key = f'team_{team}'
                if team_key not in output_dict:
                    # Create team entry dynamically
                    output_dict[team_key] = {}
                
                output_dict[team_key][str(player_id)] = {
                    'distance_km': round(distance_km, 3),
                    'distance_m': round(distance_m, 2)
                }
    
    # No rounding needed for possession percentages (already rounded to 2 decimal places)
    
    # Create output directory if it doesn't exist
    output_dir = os.path.dirname(output_path)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Save to JSON file
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(output_dict, f, indent=2, ensure_ascii=False)
    
    # Also save as CSV for easier viewing
    # Use os.path.splitext for robust path handling
    base_path, ext = os.path.splitext(output_path)
    csv_path = f'{base_path}.csv'
    with open(csv_path, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(['Team', 'Player ID', 'Distance (m)'])
        
        # Export all teams dynamically
        for team_key in sorted(output_dict.keys()):
            if team_key == 'summary':
                continue
            for player_id, data in output_dict[team_key].items():
                distance_m = data['distance_m']
                # If distance is 0, show "Not Detected" instead
                distance_display = 'Not Detected' if distance_m == 0 else distance_m
                writer.writerow([
                    team_key,
                    player_id,
                    distance_display
                ])
        
        # Add summary rows - Team Possession Percentage
        writer.writerow([])
        writer.writerow(['Summary - Team Possession Percentage', '', ''])
        
        # Write possession percentages
        if 'team_1_possession_percent' in output_dict['summary']:
            writer.writerow(['Team 1 Possession', '', f"{output_dict['summary']['team_1_possession_percent']}%"])
        if 'team_2_possession_percent' in output_dict['summary']:
            writer.writerow(['Team 2 Possession', '', f"{output_dict['summary']['team_2_possession_percent']}%"])
    
    print(f"Data output saved to {output_path} and {csv_path}")
    
    return output_dict
