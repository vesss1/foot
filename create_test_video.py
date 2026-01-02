#!/usr/bin/env python3
"""
Simple script to generate a test video (output.avi) for the VideoDataViewer
"""
import cv2
import numpy as np

def create_test_video(output_path, width=640, height=480, fps=30, duration=3):
    """Create a test video with moving objects"""
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter(output_path, fourcc, fps, (width, height))
    
    total_frames = int(fps * duration)
    
    for frame_num in range(total_frames):
        # Create a blank frame
        frame = np.zeros((height, width, 3), dtype=np.uint8)
        
        # Add gradient background
        for i in range(height):
            color_value = int(50 + (i / height) * 100)
            frame[i, :] = [color_value, color_value // 2, color_value // 3]
        
        # Add moving ball
        ball_x = int(50 + (frame_num / total_frames) * (width - 100))
        ball_y = int(height // 2 + 50 * np.sin(frame_num / 10))
        cv2.circle(frame, (ball_x, ball_y), 20, (0, 255, 255), -1)
        
        # Add some "players" (rectangles)
        for i in range(4):
            player_x = int(100 + i * 150 + (frame_num % 30) * 2)
            player_y = int(height // 2 + 100 + i * 10)
            color = (0, 255, 0) if i % 2 == 0 else (255, 0, 0)
            cv2.rectangle(frame, (player_x - 15, player_y - 30), 
                         (player_x + 15, player_y + 30), color, -1)
        
        # Add frame number
        cv2.putText(frame, f"Frame: {frame_num}", (10, 30), 
                   cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
        
        out.write(frame)
    
    out.release()
    print(f"Test video created: {output_path}")
    print(f"  Frames: {total_frames}, FPS: {fps}, Duration: {duration}s")

if __name__ == "__main__":
    output_path = "foot-Function/output_videos/output.avi"
    create_test_video(output_path)
    print("Test video generation complete!")
