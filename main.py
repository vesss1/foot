from utils import read_video, save_video, export_analysis_data
from trackers import Tracker
import cv2
import numpy as np
from team_assigner import TeamAssigner
from player_ball_assigner import PlayerBallAssigner
from camera_movement_estimator import CameraMovementEstimator
from view_transformer import ViewTransformer
from speed_and_distance_estimator import SpeedAndDistance_Estimator
import argparse
import os


def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description='Football Analysis System')
    parser.add_argument('--input', '-i', type=str, default='input_videos/08fd33_4.mp4',
                       help='Path to input video file (default: input_videos/08fd33_4.mp4)')
    parser.add_argument('--output', '-o', type=str, default='output_videos/output_video.avi',
                       help='Path to output video file (default: output_videos/output_video.avi)')
    parser.add_argument('--model', '-m', type=str, default='models/best.pt',
                       help='Path to YOLO model file (default: models/best.pt)')
    parser.add_argument('--no-cache', action='store_true',
                       help='Disable cache usage (slower but processes from scratch)')
    parser.add_argument('--no-export', action='store_true',
                       help='Disable data export to JSON/CSV files')
    parser.add_argument('--export-dir', type=str, default='output_videos',
                       help='Directory to save exported data files (default: output_videos)')
    parser.add_argument('--export-prefix', type=str, default='analysis',
                       help='Prefix for exported data files (default: analysis)')
    
    args = parser.parse_args()
    
    # Validate input file exists
    if not os.path.exists(args.input):
        print(f"Error: Input video file not found: {args.input}")
        return 1
    
    # Validate model file exists
    if not os.path.exists(args.model):
        print(f"Error: Model file not found: {args.model}")
        return 1
    
    # Ensure output directory exists
    os.makedirs(os.path.dirname(args.output) or '.', exist_ok=True)
    os.makedirs(args.export_dir, exist_ok=True)
    
    print(f"Football Analysis System")
    print(f"Input: {args.input}")
    print(f"Output: {args.output}")
    print(f"Model: {args.model}")
    print(f"Cache: {'Disabled' if args.no_cache else 'Enabled'}")
    print(f"Data Export: {'Disabled' if args.no_export else 'Enabled'}")
    print()
    
    # Read Video
    print("Reading video...")
    video_frames = read_video(args.input)
    print(f"Loaded {len(video_frames)} frames")

    # Initialize Tracker
    print("Initializing tracker...")
    tracker = Tracker(args.model)

    print("Detecting and tracking objects...")
    tracks = tracker.get_object_tracks(video_frames,
                                       read_from_stub=not args.no_cache,
                                       stub_path='stubs/track_stubs.pkl')
    # Get object positions 
    tracker.add_position_to_tracks(tracks)

    # camera movement estimator
    print("Estimating camera movement...")
    camera_movement_estimator = CameraMovementEstimator(video_frames[0])
    camera_movement_per_frame = camera_movement_estimator.get_camera_movement(video_frames,
                                                                                read_from_stub=not args.no_cache,
                                                                                stub_path='stubs/camera_movement_stub.pkl')
    camera_movement_estimator.add_adjust_positions_to_tracks(tracks,camera_movement_per_frame)


    # View Trasnformer
    print("Applying perspective transformation...")
    view_transformer = ViewTransformer()
    view_transformer.add_transformed_position_to_tracks(tracks)

    # Interpolate Ball Positions
    print("Interpolating ball positions...")
    tracks["ball"] = tracker.interpolate_ball_positions(tracks["ball"])

    # Speed and distance estimator
    print("Calculating speed and distance...")
    speed_and_distance_estimator = SpeedAndDistance_Estimator()
    speed_and_distance_estimator.add_speed_and_distance_to_tracks(tracks)

    # Assign Player Teams
    print("Assigning player teams...")
    team_assigner = TeamAssigner()
    team_assigner.assign_team_color(video_frames[0], 
                                    tracks['players'][0])
    
    for frame_num, player_track in enumerate(tracks['players']):
        for player_id, track in player_track.items():
            team = team_assigner.get_player_team(video_frames[frame_num],   
                                                 track['bbox'],
                                                 player_id)
            tracks['players'][frame_num][player_id]['team'] = team 
            tracks['players'][frame_num][player_id]['team_color'] = team_assigner.team_colors[team]

    
    # Assign Ball Aquisition
    print("Analyzing ball possession...")
    player_assigner =PlayerBallAssigner()
    team_ball_control= []
    for frame_num, player_track in enumerate(tracks['players']):
        ball_bbox = tracks['ball'][frame_num][1]['bbox']
        assigned_player = player_assigner.assign_ball_to_player(player_track, ball_bbox)

        if assigned_player != -1:
            tracks['players'][frame_num][assigned_player]['has_ball'] = True
            team_ball_control.append(tracks['players'][frame_num][assigned_player]['team'])
        else:
            team_ball_control.append(team_ball_control[-1])
    team_ball_control= np.array(team_ball_control)


    # Draw output 
    print("Generating annotated frames...")
    ## Draw object Tracks
    output_video_frames = tracker.draw_annotations(video_frames, tracks,team_ball_control)

    ## Draw Camera movement
    output_video_frames = camera_movement_estimator.draw_camera_movement(output_video_frames,camera_movement_per_frame)

    ## Draw Speed and Distance
    speed_and_distance_estimator.draw_speed_and_distance(output_video_frames,tracks)

    # Save video
    print(f"Saving output video to {args.output}...")
    save_video(output_video_frames, args.output)
    print(f"Video saved successfully!")

    # Export analysis data to structured files
    if not args.no_export:
        print("\nExporting analysis data...")
        exported_files = export_analysis_data(
            tracks=tracks,
            team_ball_control=team_ball_control,
            camera_movement_per_frame=camera_movement_per_frame,
            output_dir=args.export_dir,
            filename_prefix=args.export_prefix
        )
        
        print("\nAnalysis data exported successfully:")
        for file_type, file_path in exported_files.items():
            print(f"  - {file_type}: {file_path}")
    
    print("\nAnalysis complete!")
    return 0

if __name__ == '__main__':
    exit(main())