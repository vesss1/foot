#!/usr/bin/env python3
"""
Generate sample output data for testing the Qt UI
Creates a sample video and JSON data file
"""

import json
import os
import cv2
import numpy as np


def create_sample_video(output_path='output_videos/output_video.avi', duration_sec=10, fps=30):
    """Create a sample video for testing"""
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    width, height = 1280, 720
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter(output_path, fourcc, fps, (width, height))
    
    total_frames = int(duration_sec * fps)
    
    for i in range(total_frames):
        # Create a frame with some dynamic content
        frame = np.zeros((height, width, 3), dtype=np.uint8)
        
        # Add gradient background
        for y in range(height):
            color_val = int(255 * (y / height))
            frame[y, :] = [50, color_val // 2, 50]
        
        # Add moving circle (simulating ball)
        t = i / total_frames
        cx = int(width * t)
        cy = int(height * (0.5 + 0.3 * np.sin(t * 4 * np.pi)))
        cv2.circle(frame, (cx, cy), 30, (255, 255, 255), -1)
        
        # Add text overlay
        text = f"Football Analysis - Frame {i+1}/{total_frames}"
        cv2.putText(frame, text, (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
        
        # Add team possession indicator
        team1_text = f"Team 1: 55.2%"
        team2_text = f"Team 2: 44.8%"
        cv2.putText(frame, team1_text, (50, 100), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 102, 204), 2)
        cv2.putText(frame, team2_text, (50, 130), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (204, 0, 0), 2)
        
        # Add distance info
        distance_text = f"Total Distance: 5.42 km"
        cv2.putText(frame, distance_text, (50, 170), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 153, 0), 2)
        
        out.write(frame)
    
    out.release()
    print(f"Sample video created: {output_path}")


def create_sample_data(output_path='output_videos/data_output.json'):
    """Create sample JSON data for testing"""
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    # Sample data structure matching the actual output format
    sample_data = {
        "summary": {
            "team_1_possession_percent": 55.23,
            "team_2_possession_percent": 44.77
        },
        "team_1": {
            "1": {
                "distance_km": 0.523,
                "distance_m": 523.45
            },
            "3": {
                "distance_km": 0.678,
                "distance_m": 678.12
            },
            "5": {
                "distance_km": 0.512,
                "distance_m": 512.89
            },
            "7": {
                "distance_km": 0.601,
                "distance_m": 601.34
            },
            "9": {
                "distance_km": 0.489,
                "distance_m": 489.67
            },
            "11": {
                "distance_km": 0.534,
                "distance_m": 534.21
            }
        },
        "team_2": {
            "2": {
                "distance_km": 0.445,
                "distance_m": 445.78
            },
            "4": {
                "distance_km": 0.567,
                "distance_m": 567.23
            },
            "6": {
                "distance_km": 0.398,
                "distance_m": 398.56
            },
            "8": {
                "distance_km": 0.623,
                "distance_m": 623.89
            },
            "10": {
                "distance_km": 0.501,
                "distance_m": 501.45
            }
        }
    }
    
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(sample_data, f, indent=2, ensure_ascii=False)
    
    print(f"Sample data created: {output_path}")
    
    # Also create CSV file
    csv_path = output_path.replace('.json', '.csv')
    with open(csv_path, 'w', encoding='utf-8') as f:
        f.write("Team,Player ID,Distance (m)\n")
        f.write("team_1,1,523.45\n")
        f.write("team_1,3,678.12\n")
        f.write("team_1,5,512.89\n")
        f.write("team_1,7,601.34\n")
        f.write("team_1,9,489.67\n")
        f.write("team_1,11,534.21\n")
        f.write("team_2,2,445.78\n")
        f.write("team_2,4,567.23\n")
        f.write("team_2,6,398.56\n")
        f.write("team_2,8,623.89\n")
        f.write("team_2,10,501.45\n")
        f.write("\n")
        f.write("Summary - Team Possession Percentage,,\n")
        f.write("Team 1 Possession,,55.23%\n")
        f.write("Team 2 Possession,,44.77%\n")
    
    print(f"Sample CSV created: {csv_path}")


def main():
    """Generate all sample data"""
    print("Generating sample data for Qt UI testing...")
    create_sample_video()
    create_sample_data()
    print("\nSample data generation complete!")
    print("You can now run: python qt_ui_app.py")


if __name__ == '__main__':
    main()
