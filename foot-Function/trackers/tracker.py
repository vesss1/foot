"""
===============================================================================
OBJECT TRACKER MODULE
===============================================================================

This module implements YOLO-based object detection and tracking for football
video analysis. It detects and tracks players, referees, goalkeepers, and
the ball across video frames.

KEY COMPONENTS:
- YOLO: State-of-the-art object detection model (Ultralytics)
- ByteTrack: Multi-object tracking algorithm (via Supervision library)
- Position tracking: Converts bounding boxes to field positions
- Ball interpolation: Fills gaps in ball detection
- Annotation rendering: Draws bounding boxes and labels on video

TRACKING PROCESS:
1. Batch process frames through YOLO for detection
2. Convert detections to Supervision format
3. Apply ByteTrack for consistent object tracking across frames
4. Store tracks as dictionaries indexed by frame and track ID
5. Add position information (center or foot position)

VISUALIZATION:
- Players: Ellipse at feet with team color and track ID
- Referees: Yellow ellipse (no team assignment)
- Ball: Green triangle
- Possession: Red triangle above player with ball
- Team control: Overlay showing possession percentages
===============================================================================
"""

from ultralytics import YOLO
import supervision as sv
import pickle
import os
import numpy as np
import pandas as pd
import cv2
import sys 
sys.path.append('../')
from utils import get_center_of_bbox, get_bbox_width, get_foot_position


################################################################################
# TRACKER CLASS
################################################################################

class Tracker:
    """
    Manages object detection and tracking for football video analysis.
    
    Uses YOLO for detection and ByteTrack for maintaining consistent
    object identities across frames.
    """
    
    def __init__(self, model_path):
        """
        Initialize tracker with YOLO model.
        
        Args:
            model_path: Path to trained YOLO model (.pt file)
        """
        self.model = YOLO(model_path)  # Load YOLO detection model
        self.tracker = sv.ByteTrack()  # Initialize ByteTrack for multi-object tracking

    # =========================================================================
    # POSITION TRACKING
    # =========================================================================

    def add_position_to_tracks(sekf,tracks):
        """
        Add position information to all tracked objects.
        
        For players/referees: Uses foot position (bottom center of bbox)
        For ball: Uses center of bounding box
        
        These positions are used for distance and speed calculations.
        """
        for object, object_tracks in tracks.items():
            for frame_num, track in enumerate(object_tracks):
                for track_id, track_info in track.items():
                    bbox = track_info['bbox']
                    if object == 'ball':
                        position= get_center_of_bbox(bbox)
                    else:
                        position = get_foot_position(bbox)
                    tracks[object][frame_num][track_id]['position'] = position

    # =========================================================================
    # BALL INTERPOLATION
    # =========================================================================

    def interpolate_ball_positions(self,ball_positions):
        """
        Fill gaps in ball detection using interpolation.
        
        Ball detection can fail due to occlusion, motion blur, or small size.
        This method uses pandas interpolation to estimate ball positions in
        frames where detection failed, creating smooth trajectories.
        
        Args:
            ball_positions: List of ball tracks per frame
            
        Returns:
            Ball positions with gaps filled via interpolation
        """
        ball_positions = [x.get(1,{}).get('bbox',[]) for x in ball_positions]
        df_ball_positions = pd.DataFrame(ball_positions,columns=['x1','y1','x2','y2'])

        # Interpolate missing values
        df_ball_positions = df_ball_positions.interpolate()
        df_ball_positions = df_ball_positions.bfill()

        ball_positions = [{1: {"bbox":x}} for x in df_ball_positions.to_numpy().tolist()]

        return ball_positions

    # =========================================================================
    # OBJECT DETECTION
    # =========================================================================

    def detect_frames(self, frames):
        """
        Run YOLO detection on all video frames in batches.
        
        Batch processing improves GPU utilization and speeds up detection.
        
        Args:
            frames: List of video frames (numpy arrays)
            
        Returns:
            List of YOLO detection results (one per frame)
        """
        batch_size=20 
        detections = [] 
        for i in range(0,len(frames),batch_size):
            detections_batch = self.model.predict(frames[i:i+batch_size],conf=0.1)
            detections += detections_batch
        return detections

    # =========================================================================
    # OBJECT TRACKING
    # =========================================================================

    def get_object_tracks(self, frames, read_from_stub=False, stub_path=None):
        """
        Detect and track all objects (players, referees, ball) across frames.
        
        PROCESS:
        1. Check for cached results (stub) if enabled
        2. Run YOLO detection on all frames
        3. Convert detections to Supervision format
        4. Merge goalkeepers into player class
        5. Apply ByteTrack for consistent tracking
        6. Organize results by object type and frame
        7. Save results to stub file for future runs
        
        TRACKING DICTIONARY STRUCTURE:
        {
            "players": [frame0_dict, frame1_dict, ...],
            "referees": [frame0_dict, frame1_dict, ...],
            "ball": [frame0_dict, frame1_dict, ...]
        }
        
        Each frame_dict contains: {track_id: {"bbox": [x1,y1,x2,y2], ...}}
        
        Args:
            frames: List of video frames
            read_from_stub: If True, load from cached file
            stub_path: Path to cache file
            
        Returns:
            Dictionary with tracking data for all objects
        """
        
        if read_from_stub and stub_path is not None and os.path.exists(stub_path):
            with open(stub_path,'rb') as f:
                tracks = pickle.load(f)
            return tracks

        detections = self.detect_frames(frames)

        tracks={
            "players":[],
            "referees":[],
            "ball":[]
        }

        for frame_num, detection in enumerate(detections):
            cls_names = detection.names
            cls_names_inv = {v:k for k,v in cls_names.items()}

            # Covert to supervision Detection format
            detection_supervision = sv.Detections.from_ultralytics(detection)

            # Convert GoalKeeper to player object
            for object_ind , class_id in enumerate(detection_supervision.class_id):
                if cls_names[class_id] == "goalkeeper":
                    detection_supervision.class_id[object_ind] = cls_names_inv["player"]

            # Track Objects
            detection_with_tracks = self.tracker.update_with_detections(detection_supervision)

            tracks["players"].append({})
            tracks["referees"].append({})
            tracks["ball"].append({})

            for frame_detection in detection_with_tracks:
                bbox = frame_detection[0].tolist()
                cls_id = frame_detection[3]
                track_id = frame_detection[4]

                if cls_id == cls_names_inv['player']:
                    tracks["players"][frame_num][track_id] = {"bbox":bbox}
                
                if cls_id == cls_names_inv['referee']:
                    tracks["referees"][frame_num][track_id] = {"bbox":bbox}
            
            for frame_detection in detection_supervision:
                bbox = frame_detection[0].tolist()
                cls_id = frame_detection[3]

                if cls_id == cls_names_inv['ball']:
                    tracks["ball"][frame_num][1] = {"bbox":bbox}

        if stub_path is not None:
            with open(stub_path,'wb') as f:
                pickle.dump(tracks,f)

        return tracks
    
    # =========================================================================
    # VISUALIZATION: DRAWING METHODS
    # =========================================================================
    
    def draw_ellipse(self,frame,bbox,color,track_id=None):
        """
        Draw an ellipse at the base of a bounding box to represent a player/referee.
        
        The ellipse simulates a ground-plane marker and includes an optional
        track ID label in a colored rectangle.
        
        Args:
            frame: Video frame to draw on
            bbox: Bounding box [x1, y1, x2, y2]
            color: RGB color tuple
            track_id: Optional player/referee ID number
            
        Returns:
            Modified frame with ellipse and label drawn
        """
        y2 = int(bbox[3])
        x_center, _ = get_center_of_bbox(bbox)
        width = get_bbox_width(bbox)

        cv2.ellipse(
            frame,
            center=(x_center,y2),
            axes=(int(width), int(0.35*width)),
            angle=0.0,
            startAngle=-45,
            endAngle=235,
            color = color,
            thickness=2,
            lineType=cv2.LINE_4
        )

        rectangle_width = 40
        rectangle_height=20
        x1_rect = x_center - rectangle_width//2
        x2_rect = x_center + rectangle_width//2
        y1_rect = (y2- rectangle_height//2) +15
        y2_rect = (y2+ rectangle_height//2) +15

        if track_id is not None:
            cv2.rectangle(frame,
                          (int(x1_rect),int(y1_rect) ),
                          (int(x2_rect),int(y2_rect)),
                          color,
                          cv2.FILLED)
            
            x1_text = x1_rect+12
            if track_id > 99:
                x1_text -=10
            
            cv2.putText(
                frame,
                f"{track_id}",
                (int(x1_text),int(y1_rect+15)),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (0,0,0),
                2
            )

        return frame

    def draw_traingle(self,frame,bbox,color):
        """
        Draw a triangle above a bounding box (used for ball and possession indicator).
        
        The triangle points downward to the object it represents.
        
        Args:
            frame: Video frame to draw on
            bbox: Bounding box [x1, y1, x2, y2]
            color: RGB color tuple
            
        Returns:
            Modified frame with triangle drawn
        """
        y= int(bbox[1])
        x,_ = get_center_of_bbox(bbox)

        triangle_points = np.array([
            [x,y],
            [x-10,y-20],
            [x+10,y-20],
        ])
        cv2.drawContours(frame, [triangle_points],0,color, cv2.FILLED)
        cv2.drawContours(frame, [triangle_points],0,(0,0,0), 2)

        return frame

    def draw_team_ball_control(self,frame,frame_num,team_ball_control):
        """
        Draw team possession statistics overlay on the frame.
        
        Displays real-time possession percentages for both teams based on
        ball control history up to the current frame.
        
        Args:
            frame: Video frame to draw on
            frame_num: Current frame number
            team_ball_control: Array of team IDs (1 or 2) indicating possession per frame
            
        Returns:
            Modified frame with possession overlay
        """
        # Draw a semi-transparent rectaggle 
        overlay = frame.copy()
        cv2.rectangle(overlay, (1350, 850), (1900,970), (255,255,255), -1 )
        alpha = 0.4
        cv2.addWeighted(overlay, alpha, frame, 1 - alpha, 0, frame)

        team_ball_control_till_frame = team_ball_control[:frame_num+1]
        # Get the number of time each team had ball control
        team_1_num_frames = team_ball_control_till_frame[team_ball_control_till_frame==1].shape[0]
        team_2_num_frames = team_ball_control_till_frame[team_ball_control_till_frame==2].shape[0]
        team_1 = team_1_num_frames/(team_1_num_frames+team_2_num_frames)
        team_2 = team_2_num_frames/(team_1_num_frames+team_2_num_frames)

        cv2.putText(frame, f"Team 1 Ball Control: {team_1*100:.2f}%",(1400,900), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,0,0), 3)
        cv2.putText(frame, f"Team 2 Ball Control: {team_2*100:.2f}%",(1400,950), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,0,0), 3)

        return frame

    def draw_annotations(self,video_frames, tracks,team_ball_control):
        """
        Annotate all video frames with tracking visualizations.
        
        ANNOTATIONS INCLUDE:
        - Player ellipses with team colors and track IDs
        - Referee ellipses in yellow
        - Ball triangle in green
        - Possession indicator (red triangle) above player with ball
        - Team possession statistics overlay
        
        This is the main rendering method that creates the final annotated video.
        
        Args:
            video_frames: List of input video frames
            tracks: Complete tracking dictionary from get_object_tracks()
            team_ball_control: Array of possession data per frame
            
        Returns:
            List of annotated frames ready for video output
        """
        output_video_frames= []
        for frame_num, frame in enumerate(video_frames):
            frame = frame.copy()

            player_dict = tracks["players"][frame_num]
            ball_dict = tracks["ball"][frame_num]
            referee_dict = tracks["referees"][frame_num]

            # Draw Players
            for track_id, player in player_dict.items():
                color = player.get("team_color",(0,0,255))
                frame = self.draw_ellipse(frame, player["bbox"],color, track_id)

                if player.get('has_ball',False):
                    frame = self.draw_traingle(frame, player["bbox"],(0,0,255))

            # Draw Referee
            for _, referee in referee_dict.items():
                frame = self.draw_ellipse(frame, referee["bbox"],(0,255,255))
            
            # Draw ball 
            for track_id, ball in ball_dict.items():
                frame = self.draw_traingle(frame, ball["bbox"],(0,255,0))


            # Draw Team Ball Control
            frame = self.draw_team_ball_control(frame, frame_num, team_ball_control)

            output_video_frames.append(frame)

        return output_video_frames