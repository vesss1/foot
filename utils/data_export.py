"""
Data Export Utility Module

This module provides functions to extract and export video analysis information
to structured file formats (JSON, CSV) for further analysis.
"""

import json
import csv
import os
from datetime import datetime
import numpy as np


def export_analysis_data(tracks, team_ball_control, camera_movement_per_frame, 
                        output_dir='output_videos', filename_prefix='analysis'):
    """
    Export all analysis data to JSON and CSV files.
    
    Args:
        tracks: Dictionary containing tracking data for players, ball, and referees
        team_ball_control: Array of team ball control per frame
        camera_movement_per_frame: List of camera movement vectors
        output_dir: Directory to save the exported files
        filename_prefix: Prefix for the output filenames
    
    Returns:
        dict: Paths to the created files
    """
    os.makedirs(output_dir, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    output_files = {}
    
    # Export player statistics
    player_stats_file = os.path.join(output_dir, f'{filename_prefix}_player_stats_{timestamp}.csv')
    export_player_statistics(tracks, player_stats_file)
    output_files['player_stats'] = player_stats_file
    
    # Export ball tracking data
    ball_tracking_file = os.path.join(output_dir, f'{filename_prefix}_ball_tracking_{timestamp}.json')
    export_ball_tracking(tracks, ball_tracking_file)
    output_files['ball_tracking'] = ball_tracking_file
    
    # Export team possession statistics
    possession_file = os.path.join(output_dir, f'{filename_prefix}_team_possession_{timestamp}.json')
    export_team_possession(team_ball_control, possession_file)
    output_files['team_possession'] = possession_file
    
    # Export camera movement data
    camera_file = os.path.join(output_dir, f'{filename_prefix}_camera_movement_{timestamp}.csv')
    export_camera_movement(camera_movement_per_frame, camera_file)
    output_files['camera_movement'] = camera_file
    
    # Export comprehensive summary
    summary_file = os.path.join(output_dir, f'{filename_prefix}_summary_{timestamp}.json')
    export_comprehensive_summary(tracks, team_ball_control, camera_movement_per_frame, summary_file)
    output_files['summary'] = summary_file
    
    return output_files


def export_player_statistics(tracks, output_file):
    """
    Export player statistics to CSV file.
    
    Args:
        tracks: Dictionary containing tracking data
        output_file: Path to output CSV file
    """
    with open(output_file, 'w', newline='', encoding='utf-8') as csvfile:
        fieldnames = ['frame', 'player_id', 'team', 'bbox_x1', 'bbox_y1', 'bbox_x2', 'bbox_y2',
                     'position_x', 'position_y', 'speed_kmh', 'distance_m', 'has_ball']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        
        if 'players' not in tracks:
            return
        
        for frame_num, player_track in enumerate(tracks['players']):
            for player_id, track_info in player_track.items():
                row = {
                    'frame': frame_num,
                    'player_id': player_id,
                    'team': track_info.get('team', 'N/A'),
                    'bbox_x1': track_info['bbox'][0] if 'bbox' in track_info else '',
                    'bbox_y1': track_info['bbox'][1] if 'bbox' in track_info else '',
                    'bbox_x2': track_info['bbox'][2] if 'bbox' in track_info else '',
                    'bbox_y2': track_info['bbox'][3] if 'bbox' in track_info else '',
                    'position_x': track_info['position'][0] if 'position' in track_info else '',
                    'position_y': track_info['position'][1] if 'position' in track_info else '',
                    'speed_kmh': f"{track_info.get('speed', 0):.2f}",
                    'distance_m': f"{track_info.get('distance', 0):.2f}",
                    'has_ball': track_info.get('has_ball', False)
                }
                writer.writerow(row)


def export_ball_tracking(tracks, output_file):
    """
    Export ball tracking data to JSON file.
    
    Args:
        tracks: Dictionary containing tracking data
        output_file: Path to output JSON file
    """
    ball_data = []
    
    if 'ball' not in tracks:
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(ball_data, f, indent=2)
        return
    
    for frame_num, ball_track in enumerate(tracks['ball']):
        if 1 in ball_track:  # Ball typically has ID 1
            track_info = ball_track[1]
            frame_data = {
                'frame': frame_num,
                'bbox': track_info.get('bbox', []),
                'position': track_info.get('position', [])
            }
            ball_data.append(frame_data)
    
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(ball_data, f, indent=2)


def export_team_possession(team_ball_control, output_file):
    """
    Export team possession statistics to JSON file.
    
    Args:
        team_ball_control: Array of team ball control per frame
        output_file: Path to output JSON file
    """
    if team_ball_control is None or len(team_ball_control) == 0:
        possession_data = {
            'team_1_frames': 0,
            'team_2_frames': 0,
            'team_1_percentage': 0,
            'team_2_percentage': 0,
            'total_frames': 0,
            'possession_by_frame': []
        }
    else:
        team_ball_control = np.array(team_ball_control)
        team_1_frames = int(np.sum(team_ball_control == 1))
        team_2_frames = int(np.sum(team_ball_control == 2))
        total_frames = len(team_ball_control)
        
        possession_data = {
            'team_1_frames': team_1_frames,
            'team_2_frames': team_2_frames,
            'team_1_percentage': round((team_1_frames / total_frames * 100) if total_frames > 0 else 0, 2),
            'team_2_percentage': round((team_2_frames / total_frames * 100) if total_frames > 0 else 0, 2),
            'total_frames': total_frames,
            'possession_by_frame': [int(x) for x in team_ball_control.tolist()]
        }
    
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(possession_data, f, indent=2)


def export_camera_movement(camera_movement_per_frame, output_file):
    """
    Export camera movement data to CSV file.
    
    Args:
        camera_movement_per_frame: List of camera movement vectors
        output_file: Path to output CSV file
    """
    with open(output_file, 'w', newline='', encoding='utf-8') as csvfile:
        fieldnames = ['frame', 'movement_x', 'movement_y']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        
        for frame_num, movement in enumerate(camera_movement_per_frame):
            row = {
                'frame': frame_num,
                'movement_x': movement[0] if movement is not None and len(movement) > 0 else 0,
                'movement_y': movement[1] if movement is not None and len(movement) > 1 else 0
            }
            writer.writerow(row)


def export_comprehensive_summary(tracks, team_ball_control, camera_movement_per_frame, output_file):
    """
    Export a comprehensive summary of all analysis data to JSON file.
    
    Args:
        tracks: Dictionary containing tracking data
        team_ball_control: Array of team ball control per frame
        camera_movement_per_frame: List of camera movement vectors
        output_file: Path to output JSON file
    """
    summary = {
        'metadata': {
            'export_timestamp': datetime.now().isoformat(),
            'total_frames': len(tracks.get('players', [])),
        },
        'player_summary': {},
        'team_possession': {},
        'ball_summary': {},
        'camera_movement_summary': {}
    }
    
    # Player summary
    if 'players' in tracks:
        player_stats = {}
        for frame_num, player_track in enumerate(tracks['players']):
            for player_id, track_info in player_track.items():
                if player_id not in player_stats:
                    player_stats[player_id] = {
                        'team': track_info.get('team', 'N/A'),
                        'max_speed': 0,
                        'total_distance': 0,
                        'frames_with_ball': 0,
                        'total_frames': 0
                    }
                
                player_stats[player_id]['total_frames'] += 1
                
                speed = track_info.get('speed', 0)
                if speed > player_stats[player_id]['max_speed']:
                    player_stats[player_id]['max_speed'] = speed
                
                distance = track_info.get('distance', 0)
                if distance > player_stats[player_id]['total_distance']:
                    player_stats[player_id]['total_distance'] = distance
                
                if track_info.get('has_ball', False):
                    player_stats[player_id]['frames_with_ball'] += 1
        
        # Convert to serializable format
        summary['player_summary'] = {
            str(player_id): {
                'team': stats['team'],
                'max_speed_kmh': round(stats['max_speed'], 2),
                'total_distance_m': round(stats['total_distance'], 2),
                'frames_with_ball': stats['frames_with_ball'],
                'total_frames': stats['total_frames']
            }
            for player_id, stats in player_stats.items()
        }
    
    # Team possession summary
    if team_ball_control is not None and len(team_ball_control) > 0:
        team_ball_control = np.array(team_ball_control)
        team_1_frames = int(np.sum(team_ball_control == 1))
        team_2_frames = int(np.sum(team_ball_control == 2))
        total_frames = len(team_ball_control)
        
        summary['team_possession'] = {
            'team_1': {
                'frames': team_1_frames,
                'percentage': round((team_1_frames / total_frames * 100) if total_frames > 0 else 0, 2)
            },
            'team_2': {
                'frames': team_2_frames,
                'percentage': round((team_2_frames / total_frames * 100) if total_frames > 0 else 0, 2)
            }
        }
    
    # Ball summary
    if 'ball' in tracks:
        ball_detected_frames = sum(1 for ball_track in tracks['ball'] if 1 in ball_track)
        summary['ball_summary'] = {
            'total_detections': ball_detected_frames,
            'detection_rate': round((ball_detected_frames / len(tracks['ball']) * 100) if len(tracks['ball']) > 0 else 0, 2)
        }
    
    # Camera movement summary
    if camera_movement_per_frame:
        total_movement_x = sum(abs(m[0]) for m in camera_movement_per_frame if m is not None and len(m) > 0)
        total_movement_y = sum(abs(m[1]) for m in camera_movement_per_frame if m is not None and len(m) > 1)
        
        summary['camera_movement_summary'] = {
            'total_frames': len(camera_movement_per_frame),
            'total_movement_x': round(total_movement_x, 2),
            'total_movement_y': round(total_movement_y, 2)
        }
    
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(summary, f, indent=2, ensure_ascii=False)
