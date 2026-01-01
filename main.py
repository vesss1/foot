#!/usr/bin/env python3
"""
Football Analysis - Main Pipeline
Robust, production-safe video analysis with comprehensive error handling.
"""

import os
import sys
import logging
from pathlib import Path
from typing import Optional, List, Dict, Any

import cv2
import numpy as np

from utils import read_video, save_video, output_data
from trackers import Tracker
from team_assigner import TeamAssigner
from player_ball_assigner import PlayerBallAssigner
from camera_movement_estimator import CameraMovementEstimator
from view_transformer import ViewTransformer
from speed_and_distance_estimator import SpeedAndDistance_Estimator

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class VideoAnalysisPipeline:
    """Production-safe video analysis pipeline with fail-fast error handling."""
    
    def __init__(self, 
                 input_video_path: str,
                 model_path: str,
                 output_dir: str = 'output_videos',
                 use_stubs: bool = True):
        """
        Initialize the video analysis pipeline.
        
        Args:
            input_video_path: Path to input video file
            model_path: Path to YOLO model file
            output_dir: Directory for output files
            use_stubs: Whether to use cached stub files for faster processing
        """
        self.input_video_path = input_video_path
        self.model_path = model_path
        self.output_dir = output_dir
        self.use_stubs = use_stubs
        
        # Validate inputs early (fail-fast)
        self._validate_inputs()
        
        # Create output directory
        self._setup_output_directory()
        
    def _validate_inputs(self) -> None:
        """Validate all required inputs exist and are accessible."""
        # Check input video
        if not os.path.exists(self.input_video_path):
            raise FileNotFoundError(
                f"Input video not found: {self.input_video_path}"
            )
        
        # Check model file
        if not os.path.exists(self.model_path):
            raise FileNotFoundError(
                f"Model file not found: {self.model_path}"
            )
        
        # Verify input video is readable
        cap = cv2.VideoCapture(self.input_video_path)
        if not cap.isOpened():
            raise IOError(
                f"Cannot open video file: {self.input_video_path}"
            )
        cap.release()
        
        logger.info("Input validation passed")
    
    def _setup_output_directory(self) -> None:
        """Create output directory if it doesn't exist."""
        try:
            os.makedirs(self.output_dir, exist_ok=True)
            logger.info(f"Output directory ready: {self.output_dir}")
        except Exception as e:
            raise IOError(
                f"Failed to create output directory {self.output_dir}: {e}"
            )
    
    def _read_video(self) -> List[np.ndarray]:
        """Read video frames with error handling."""
        try:
            logger.info(f"Reading video: {self.input_video_path}")
            frames = read_video(self.input_video_path)
            
            if not frames:
                raise ValueError("No frames read from video")
            
            logger.info(f"Successfully read {len(frames)} frames")
            return frames
        except Exception as e:
            raise RuntimeError(f"Failed to read video: {e}")
    
    def _initialize_tracker(self) -> Tracker:
        """Initialize object tracker with error handling."""
        try:
            logger.info("Initializing tracker")
            tracker = Tracker(self.model_path)
            return tracker
        except Exception as e:
            raise RuntimeError(f"Failed to initialize tracker: {e}")
    
    def _get_object_tracks(self, 
                          tracker: Tracker, 
                          frames: List[np.ndarray]) -> Dict[str, Any]:
        """Get object tracks with stub support."""
        try:
            stub_path = 'stubs/track_stubs.pkl' if self.use_stubs else None
            logger.info("Getting object tracks")
            
            tracks = tracker.get_object_tracks(
                frames,
                read_from_stub=self.use_stubs,
                stub_path=stub_path
            )
            
            if not tracks or 'players' not in tracks:
                raise ValueError("Invalid tracks data structure")
            
            logger.info("Object tracking complete")
            return tracks
        except Exception as e:
            raise RuntimeError(f"Failed to get object tracks: {e}")
    
    def _process_camera_movement(self,
                                 frames: List[np.ndarray],
                                 tracks: Dict[str, Any]) -> List[np.ndarray]:
        """Process camera movement estimation."""
        try:
            logger.info("Processing camera movement")
            
            estimator = CameraMovementEstimator(frames[0])
            stub_path = 'stubs/camera_movement_stub.pkl' if self.use_stubs else None
            
            camera_movement = estimator.get_camera_movement(
                frames,
                read_from_stub=self.use_stubs,
                stub_path=stub_path
            )
            
            estimator.add_adjust_positions_to_tracks(tracks, camera_movement)
            logger.info("Camera movement processing complete")
            
            return camera_movement
        except Exception as e:
            raise RuntimeError(f"Failed to process camera movement: {e}")
    
    def _process_view_transformation(self, tracks: Dict[str, Any]) -> None:
        """Apply view transformation to tracks."""
        try:
            logger.info("Applying view transformation")
            transformer = ViewTransformer()
            transformer.add_transformed_position_to_tracks(tracks)
            logger.info("View transformation complete")
        except Exception as e:
            raise RuntimeError(f"Failed to apply view transformation: {e}")
    
    def _interpolate_ball_positions(self, 
                                    tracker: Tracker,
                                    tracks: Dict[str, Any]) -> None:
        """Interpolate ball positions."""
        try:
            logger.info("Interpolating ball positions")
            tracks["ball"] = tracker.interpolate_ball_positions(tracks["ball"])
            logger.info("Ball position interpolation complete")
        except Exception as e:
            raise RuntimeError(f"Failed to interpolate ball positions: {e}")
    
    def _estimate_speed_and_distance(self, tracks: Dict[str, Any]) -> None:
        """Calculate speed and distance metrics."""
        try:
            logger.info("Calculating speed and distance")
            estimator = SpeedAndDistance_Estimator()
            estimator.add_speed_and_distance_to_tracks(tracks)
            logger.info("Speed and distance calculation complete")
        except Exception as e:
            raise RuntimeError(f"Failed to calculate speed and distance: {e}")
    
    def _assign_teams(self, 
                     frames: List[np.ndarray],
                     tracks: Dict[str, Any]) -> TeamAssigner:
        """Assign players to teams."""
        try:
            logger.info("Assigning player teams")
            
            if not tracks['players'] or not tracks['players'][0]:
                raise ValueError("No players detected in first frame")
            
            assigner = TeamAssigner()
            assigner.assign_team_color(frames[0], tracks['players'][0])
            
            for frame_num, player_track in enumerate(tracks['players']):
                for player_id, track in player_track.items():
                    team = assigner.get_player_team(
                        frames[frame_num],
                        track['bbox'],
                        player_id
                    )
                    tracks['players'][frame_num][player_id]['team'] = team
                    tracks['players'][frame_num][player_id]['team_color'] = \
                        assigner.team_colors[team]
            
            logger.info("Team assignment complete")
            return assigner
        except Exception as e:
            raise RuntimeError(f"Failed to assign teams: {e}")
    
    def _assign_ball_possession(self, tracks: Dict[str, Any]) -> np.ndarray:
        """Determine ball possession for each frame."""
        try:
            logger.info("Assigning ball possession")
            
            assigner = PlayerBallAssigner()
            team_ball_control = []
            
            for frame_num, player_track in enumerate(tracks['players']):
                if 1 not in tracks['ball'][frame_num]:
                    # No ball detected, use previous possession
                    if team_ball_control:
                        team_ball_control.append(team_ball_control[-1])
                    else:
                        team_ball_control.append(0)
                    continue
                
                ball_bbox = tracks['ball'][frame_num][1]['bbox']
                assigned_player = assigner.assign_ball_to_player(
                    player_track, 
                    ball_bbox
                )
                
                if assigned_player != -1:
                    tracks['players'][frame_num][assigned_player]['has_ball'] = True
                    team_ball_control.append(
                        tracks['players'][frame_num][assigned_player]['team']
                    )
                else:
                    # No assignment, use previous
                    if team_ball_control:
                        team_ball_control.append(team_ball_control[-1])
                    else:
                        team_ball_control.append(0)
            
            logger.info("Ball possession assignment complete")
            return np.array(team_ball_control)
        except Exception as e:
            raise RuntimeError(f"Failed to assign ball possession: {e}")
    
    def _draw_annotations(self,
                         tracker: Tracker,
                         frames: List[np.ndarray],
                         tracks: Dict[str, Any],
                         team_ball_control: np.ndarray,
                         camera_movement: List[np.ndarray]) -> List[np.ndarray]:
        """Draw all annotations on video frames."""
        try:
            logger.info("Drawing annotations")
            
            # Draw object tracks
            output_frames = tracker.draw_annotations(
                frames, 
                tracks, 
                team_ball_control
            )
            
            # Draw camera movement
            estimator = CameraMovementEstimator(frames[0])
            output_frames = estimator.draw_camera_movement(
                output_frames,
                camera_movement
            )
            
            # Draw speed and distance
            speed_estimator = SpeedAndDistance_Estimator()
            speed_estimator.draw_speed_and_distance(output_frames, tracks)
            
            logger.info("Annotation drawing complete")
            return output_frames
        except Exception as e:
            raise RuntimeError(f"Failed to draw annotations: {e}")
    
    def _save_output_video(self, frames: List[np.ndarray]) -> str:
        """Save output video with robust error handling."""
        try:
            output_path = os.path.join(self.output_dir, 'output_video.avi')
            logger.info(f"Saving output video to: {output_path}")
            
            if not frames:
                raise ValueError("No frames to save")
            
            save_video(frames, output_path)
            
            # Verify output file was created
            if not os.path.exists(output_path):
                raise IOError("Output video file was not created")
            
            file_size = os.path.getsize(output_path)
            logger.info(
                f"Output video saved successfully ({file_size / 1024 / 1024:.2f} MB)"
            )
            
            return output_path
        except Exception as e:
            raise RuntimeError(f"Failed to save output video: {e}")
    
    def _save_output_data(self, tracks: Dict[str, Any], team_ball_control: np.ndarray) -> str:
        """Save output data with error handling."""
        try:
            output_path = os.path.join(self.output_dir, 'data_output.json')
            logger.info(f"Saving output data to: {output_path}")
            
            output_data(tracks, output_path, team_ball_control)
            
            # Verify output files were created
            if not os.path.exists(output_path):
                raise IOError("Output data file was not created")
            
            csv_path = output_path.replace('.json', '.csv')
            if not os.path.exists(csv_path):
                raise IOError("Output CSV file was not created")
            
            logger.info("Output data saved successfully")
            return output_path
        except Exception as e:
            raise RuntimeError(f"Failed to save output data: {e}")
    
    def run(self) -> None:
        """Execute the complete video analysis pipeline."""
        try:
            logger.info("="*60)
            logger.info("Starting Football Analysis Pipeline")
            logger.info("="*60)
            
            # Read video
            video_frames = self._read_video()
            
            # Initialize tracker
            tracker = self._initialize_tracker()
            
            # Get object tracks
            tracks = self._get_object_tracks(tracker, video_frames)
            
            # Add positions to tracks
            tracker.add_position_to_tracks(tracks)
            
            # Process camera movement
            camera_movement = self._process_camera_movement(video_frames, tracks)
            
            # Apply view transformation
            self._process_view_transformation(tracks)
            
            # Interpolate ball positions
            self._interpolate_ball_positions(tracker, tracks)
            
            # Calculate speed and distance
            self._estimate_speed_and_distance(tracks)
            
            # Assign teams
            self._assign_teams(video_frames, tracks)
            
            # Assign ball possession
            team_ball_control = self._assign_ball_possession(tracks)
            
            # Draw annotations
            output_frames = self._draw_annotations(
                tracker,
                video_frames,
                tracks,
                team_ball_control,
                camera_movement
            )
            
            # Save outputs
            video_path = self._save_output_video(output_frames)
            data_path = self._save_output_data(tracks, team_ball_control)
            
            logger.info("="*60)
            logger.info("Pipeline completed successfully!")
            logger.info(f"Video output: {video_path}")
            logger.info(f"Data output: {data_path}")
            logger.info("="*60)
            
        except Exception as e:
            logger.error("="*60)
            logger.error(f"Pipeline failed: {e}")
            logger.error("="*60)
            raise


def main():
    """Main entry point with command-line argument support."""
    try:
        # Configuration
        input_video = 'input_videos/08fd33_4.mp4'
        model_file = 'models/best.pt'
        output_directory = 'output_videos'
        use_cached_stubs = True
        
        # Create and run pipeline
        pipeline = VideoAnalysisPipeline(
            input_video_path=input_video,
            model_path=model_file,
            output_dir=output_directory,
            use_stubs=use_cached_stubs
        )
        
        pipeline.run()
        
        return 0
        
    except KeyboardInterrupt:
        logger.warning("Process interrupted by user")
        return 130
    except Exception as e:
        logger.exception(f"Fatal error: {e}")
        return 1


if __name__ == '__main__':
    sys.exit(main())