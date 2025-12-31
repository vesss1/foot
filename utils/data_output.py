import json
import csv


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
    # Initialize data structure
    output_dict = {
        'team_1': {},
        'team_2': {},
        'summary': {
            'team_1_total_distance_km': 0,
            'team_2_total_distance_km': 0
        }
    }
    
    # Process player tracks
    if 'players' in tracks:
        # Get the last frame to get final distance values
        last_frame = len(tracks['players']) - 1
        
        for player_id, player_data in tracks['players'][last_frame].items():
            # Get player team (default to 1 if not available)
            team = player_data.get('team', 1)
            
            # Get distance in meters and convert to kilometers
            distance_m = player_data.get('distance', 0)
            distance_km = distance_m / 1000
            
            # Organize by team
            team_key = f'team_{team}'
            if team_key in output_dict:
                output_dict[team_key][str(player_id)] = {
                    'distance_km': round(distance_km, 3),
                    'distance_m': round(distance_m, 2)
                }
                
                # Add to team total
                output_dict['summary'][f'{team_key}_total_distance_km'] += distance_km
    
    # Round summary values
    output_dict['summary']['team_1_total_distance_km'] = round(
        output_dict['summary']['team_1_total_distance_km'], 3
    )
    output_dict['summary']['team_2_total_distance_km'] = round(
        output_dict['summary']['team_2_total_distance_km'], 3
    )
    
    # Save to JSON file
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(output_dict, f, indent=2, ensure_ascii=False)
    
    # Also save as CSV for easier viewing
    csv_path = output_path.replace('.json', '.csv')
    with open(csv_path, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(['Team', 'Player ID', 'Distance (km)', 'Distance (m)'])
        
        for team_key in ['team_1', 'team_2']:
            for player_id, data in output_dict[team_key].items():
                writer.writerow([
                    team_key,
                    player_id,
                    data['distance_km'],
                    data['distance_m']
                ])
        
        # Add summary row
        writer.writerow([])
        writer.writerow(['Summary', '', '', ''])
        writer.writerow(['Team 1 Total', '', output_dict['summary']['team_1_total_distance_km'], ''])
        writer.writerow(['Team 2 Total', '', output_dict['summary']['team_2_total_distance_km'], ''])
    
    print(f"Data output saved to {output_path} and {csv_path}")
    
    return output_dict
