#!/usr/bin/env python3
"""
Generate a visualization/mockup of the Qt UI
Since we're in a headless environment, create a composite image showing the UI layout
"""

import cv2
import numpy as np
import json
import os


def create_ui_mockup():
    """Create a visual representation of the UI"""
    # Create canvas
    width, height = 1400, 800
    canvas = np.ones((height, width, 3), dtype=np.uint8) * 240
    
    # Draw left panel - Video area
    video_x, video_y = 20, 60
    video_w, video_h = 900, 650
    
    # Video panel background
    cv2.rectangle(canvas, (video_x, video_y), (video_x + video_w, video_y + video_h), (200, 200, 200), -1)
    cv2.rectangle(canvas, (video_x, video_y), (video_x + video_w, video_y + video_h), (100, 100, 100), 2)
    
    # Add title
    cv2.putText(canvas, "Video Output", (video_x + 350, 40), cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 0, 0), 2)
    
    # Simulate video frame with sample content
    frame_img = cv2.imread('output_videos/output_video.avi')
    if frame_img is not None and len(frame_img.shape) == 3:
        # Get first frame from video
        cap = cv2.VideoCapture('output_videos/output_video.avi')
        ret, frame_img = cap.read()
        cap.release()
        
        if ret:
            # Resize to fit
            frame_resized = cv2.resize(frame_img, (video_w - 20, 500))
            canvas[video_y + 10:video_y + 510, video_x + 10:video_x + video_w - 10] = frame_resized
    else:
        # Draw placeholder
        cv2.putText(canvas, "Football Analysis Video", 
                   (video_x + 250, video_y + 250), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)
    
    # Draw video controls
    control_y = video_y + 520
    cv2.rectangle(canvas, (video_x + 10, control_y), (video_x + 80, control_y + 40), (70, 130, 180), -1)
    cv2.putText(canvas, "Play", (video_x + 25, control_y + 27), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
    
    cv2.rectangle(canvas, (video_x + 100, control_y), (video_x + 170, control_y + 40), (70, 130, 180), -1)
    cv2.putText(canvas, "Stop", (video_x + 112, control_y + 27), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
    
    # Slider
    cv2.rectangle(canvas, (video_x + 190, control_y + 15), (video_x + 850, control_y + 25), (150, 150, 150), -1)
    cv2.circle(canvas, (video_x + 350, control_y + 20), 12, (70, 130, 180), -1)
    
    # Frame info
    cv2.putText(canvas, "Frame 150/300 | Time: 5.0s / 10.0s", 
               (video_x + 280, control_y + 70), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 0), 1)
    
    # Load button
    cv2.rectangle(canvas, (video_x + 10, control_y + 90), (video_x + 200, control_y + 130), (70, 130, 180), -1)
    cv2.putText(canvas, "Load Video File", (video_x + 30, control_y + 117), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
    
    # Draw right panel - Statistics
    stats_x = 940
    stats_w = 440
    
    cv2.putText(canvas, "Match Statistics", (stats_x + 110, 40), 
               cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 0, 0), 2)
    
    # Load statistics data
    try:
        with open('output_videos/data_output.json', 'r') as f:
            stats_data = json.load(f)
        
        summary = stats_data.get('summary', {})
        team1_poss = summary.get('team_1_possession_percent', 0)
        team2_poss = summary.get('team_2_possession_percent', 0)
        
        # Calculate distances
        total_distance = 0
        player_count = 0
        for key in stats_data:
            if key.startswith('team_'):
                team_data = stats_data[key]
                if isinstance(team_data, dict):
                    for player_id, player_data in team_data.items():
                        if isinstance(player_data, dict):
                            distance_km = player_data.get('distance_km', 0)
                            total_distance += distance_km
                            player_count += 1
        
        avg_distance = total_distance / player_count if player_count > 0 else 0
        
    except:
        team1_poss = 55.2
        team2_poss = 44.8
        total_distance = 5.87
        avg_distance = 0.53
    
    # Ball Possession Section
    y_offset = 80
    cv2.rectangle(canvas, (stats_x, y_offset), (stats_x + stats_w, y_offset + 120), (220, 220, 220), -1)
    cv2.rectangle(canvas, (stats_x, y_offset), (stats_x + stats_w, y_offset + 120), (100, 100, 100), 2)
    cv2.putText(canvas, "Ball Possession", (stats_x + 120, y_offset + 30), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 0, 0), 2)
    
    cv2.putText(canvas, "Team 1:", (stats_x + 20, y_offset + 65), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 1)
    cv2.putText(canvas, f"{team1_poss:.1f}%", (stats_x + 200, y_offset + 70), 
               cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 102, 204), 2)
    
    cv2.putText(canvas, "Team 2:", (stats_x + 20, y_offset + 105), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 1)
    cv2.putText(canvas, f"{team2_poss:.1f}%", (stats_x + 200, y_offset + 110), 
               cv2.FONT_HERSHEY_SIMPLEX, 1.2, (204, 0, 0), 2)
    
    # Running Statistics Section
    y_offset = 220
    cv2.rectangle(canvas, (stats_x, y_offset), (stats_x + stats_w, y_offset + 120), (220, 220, 220), -1)
    cv2.rectangle(canvas, (stats_x, y_offset), (stats_x + stats_w, y_offset + 120), (100, 100, 100), 2)
    cv2.putText(canvas, "Running Statistics", (stats_x + 100, y_offset + 30), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 0, 0), 2)
    
    cv2.putText(canvas, "Total Distance:", (stats_x + 20, y_offset + 65), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 1)
    cv2.putText(canvas, f"{total_distance:.2f} km", (stats_x + 220, y_offset + 70), 
               cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 153, 0), 2)
    
    cv2.putText(canvas, "Avg per Player:", (stats_x + 20, y_offset + 105), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 1)
    cv2.putText(canvas, f"{avg_distance:.2f} km", (stats_x + 220, y_offset + 105), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 102, 0), 2)
    
    # Time Statistics Section
    y_offset = 360
    cv2.rectangle(canvas, (stats_x, y_offset), (stats_x + stats_w, y_offset + 120), (220, 220, 220), -1)
    cv2.rectangle(canvas, (stats_x, y_offset), (stats_x + stats_w, y_offset + 120), (100, 100, 100), 2)
    cv2.putText(canvas, "Time Statistics", (stats_x + 120, y_offset + 30), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 0, 0), 2)
    
    cv2.putText(canvas, "Video Duration:", (stats_x + 20, y_offset + 65), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 1)
    cv2.putText(canvas, "10.0 s", (stats_x + 250, y_offset + 65), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 1)
    
    cv2.putText(canvas, "Avg Speed:", (stats_x + 20, y_offset + 100), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 1)
    cv2.putText(canvas, "2.11 km/h", (stats_x + 250, y_offset + 100), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 1)
    
    # Refresh button
    y_offset = 500
    cv2.rectangle(canvas, (stats_x + 50, y_offset), (stats_x + stats_w - 50, y_offset + 45), (70, 130, 180), -1)
    cv2.putText(canvas, "Refresh Statistics", (stats_x + 100, y_offset + 30), 
               cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
    
    # Save mockup
    output_path = 'output_videos/qt_ui_mockup.png'
    cv2.imwrite(output_path, canvas)
    print(f"UI mockup saved to: {output_path}")
    
    return output_path


if __name__ == '__main__':
    create_ui_mockup()
