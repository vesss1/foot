import json
import csv
import os


def output_data(tracks, output_path='output_videos/data_output.json'):
    """
    Output numerical data from video analysis including:
    - Distance covered by each player (in kilometers)
    - Data separated by team
    
    Args:
        tracks: Dictionary containing tracking information for players
        output_path: Path to save the output data (default: 'output_videos/data_output.json')
    
    Returns:
        Dictionary containing the organized data
    """
    # Initialize data structure with summary only
    output_dict = {
        'summary': {}
    }
    
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
                    # Create team entry and summary entry dynamically
                    output_dict[team_key] = {}
                    output_dict['summary'][f'{team_key}_total_distance_km'] = 0
                
                output_dict[team_key][str(player_id)] = {
                    'distance_km': round(distance_km, 3),
                    'distance_m': round(distance_m, 2)
                }
                
                # Add to team total
                output_dict['summary'][f'{team_key}_total_distance_km'] += distance_km
    
    # Round summary values
    for key in output_dict['summary']:
        output_dict['summary'][key] = round(output_dict['summary'][key], 3)
    
    # Save to JSON file
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(output_dict, f, indent=2, ensure_ascii=False)
    
    # Also save as CSV for easier viewing
    # Use os.path.splitext for robust path handling
    base_path, ext = os.path.splitext(output_path)
    csv_path = f'{base_path}.csv'
    with open(csv_path, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(['Team', 'Player ID', 'Distance (km)', 'Distance (m)'])
        
        # Export all teams dynamically
        for team_key in sorted(output_dict.keys()):
            if team_key == 'summary':
                continue
            for player_id, data in output_dict[team_key].items():
                writer.writerow([
                    team_key,
                    player_id,
                    data['distance_km'],
                    data['distance_m']
                ])
        
        # Add summary rows
        writer.writerow([])
        writer.writerow(['Summary', '', '', ''])
        for team_key in sorted(output_dict.keys()):
            if team_key == 'summary':
                continue
            summary_key = f'{team_key}_total_distance_km'
            if summary_key in output_dict['summary']:
                team_label = team_key.replace('_', ' ').title()
                writer.writerow([f'{team_label} Total', '', output_dict['summary'][summary_key], ''])
    
    print(f"Data output saved to {output_path} and {csv_path}")
    
    return output_dict
