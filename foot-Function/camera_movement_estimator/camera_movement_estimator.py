"""
===============================================================================
CAMERA MOVEMENT ESTIMATOR MODULE
===============================================================================

This module estimates camera motion (pan, tilt, zoom) using optical flow
and compensates object positions accordingly. This is essential for accurate
speed and distance calculations.

PROBLEM:
Camera movement causes objects to move in the frame even if they're stationary
in the real world. This would produce incorrect speed and distance measurements.

SOLUTION:
Track fixed features in the background (e.g., field markings, stadium elements)
and measure their movement across frames. This movement represents camera motion.
Subtract camera motion from all object positions to get true object movement.

ALGORITHM:
1. Detect good features to track (corners) in first frame
2. Track features across frames using Lucas-Kanade optical flow
3. Calculate median feature movement per frame (camera motion)
4. Subtract camera motion from all object positions

FEATURE SELECTION:
Features are detected in specific regions likely to contain static elements:
- Left edge (x: 0-20): Stadium/crowd
- Right edge (x: 900-1050): Stadium/crowd
These regions typically don't contain moving players/ball.

OUTPUT:
Camera movement per frame as [dx, dy] arrays, where:
- dx: horizontal camera movement in pixels
- dy: vertical camera movement in pixels
===============================================================================
"""

import pickle
import cv2
import numpy as np
import os
import sys 
sys.path.append('../')
from utils import measure_distance,measure_xy_distance


################################################################################
# CAMERA MOVEMENT ESTIMATOR CLASS
################################################################################

class CameraMovementEstimator():
    """
    Estimates camera motion using optical flow on background features.
    """
    
    def __init__(self,frame):
        """
        Initialize camera movement estimator with first frame.
        
        Sets up optical flow parameters and feature detection settings.
        
        Args:
            frame: First video frame for initialization
        """
        self.minimum_distance = 5  # Minimum movement threshold (pixels)

        # Lucas-Kanade optical flow parameters
        self.lk_params = dict(
            winSize = (15,15),        # Size of search window at each pyramid level
            maxLevel = 2,              # Maximum pyramid levels
            criteria = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT,10,0.03)
        )

        # Prepare feature mask to detect only in specific regions
        first_frame_grayscale = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        mask_features = np.zeros_like(first_frame_grayscale)
        mask_features[:,0:20] = 1      # Left edge region
        mask_features[:,900:1050] = 1  # Right edge region (adjust based on video resolution)

        # Good features to track parameters
        self.features = dict(
            maxCorners = 100,      # Maximum number of corners to detect
            qualityLevel = 0.3,    # Minimum quality of corners
            minDistance =3,        # Minimum distance between corners
            blockSize = 7,         # Size of averaging block
            mask = mask_features   # Only detect in masked regions
        )

    # =========================================================================
    # POSITION ADJUSTMENT
    # =========================================================================

    def add_adjust_positions_to_tracks(self,tracks, camera_movement_per_frame):
        """
        Compensate all object positions for camera movement.
        
        Subtracts camera motion from each object's position to get
        camera-independent positions for accurate distance/speed calculation.
        
        Args:
            tracks: Tracking dictionary for all objects
            camera_movement_per_frame: List of [dx, dy] camera movement per frame
        """
        for object, object_tracks in tracks.items():
            for frame_num, track in enumerate(object_tracks):
                for track_id, track_info in track.items():
                    position = track_info['position']
                    camera_movement = camera_movement_per_frame[frame_num]
                    position_adjusted = (position[0]-camera_movement[0],position[1]-camera_movement[1])
                    tracks[object][frame_num][track_id]['position_adjusted'] = position_adjusted
                    

    # =========================================================================
    # CAMERA MOVEMENT ESTIMATION
    # =========================================================================

    def get_camera_movement(self,frames,read_from_stub=False, stub_path=None):
        """
        Calculate camera movement for all frames using optical flow.
        
        PROCESS:
        1. Load from cache (stub) if available
        2. Detect good features (corners) in first frame
        3. For each subsequent frame:
           - Track features using Lucas-Kanade optical flow
           - Calculate movement of each feature
           - Use median movement as camera motion (robust to outliers)
        4. Save results to cache for future runs
        
        ROBUSTNESS:
        - Uses median instead of mean to handle outliers (moving objects)
        - Only tracks features in specific regions (background)
        - Filters out large movements (likely not camera motion)
        
        Args:
            frames: List of all video frames
            read_from_stub: If True, load from cached file
            stub_path: Path to cache file
            
        Returns:
            List of [dx, dy] camera movement vectors per frame
        """
        # Read the stub 
        if read_from_stub and stub_path is not None and os.path.exists(stub_path):
            with open(stub_path,'rb') as f:
                return pickle.load(f)

        camera_movement = [[0,0]]*len(frames)

        old_gray = cv2.cvtColor(frames[0],cv2.COLOR_BGR2GRAY)
        old_features = cv2.goodFeaturesToTrack(old_gray,**self.features)

        for frame_num in range(1,len(frames)):
            frame_gray = cv2.cvtColor(frames[frame_num],cv2.COLOR_BGR2GRAY)
            new_features, _,_ = cv2.calcOpticalFlowPyrLK(old_gray,frame_gray,old_features,None,**self.lk_params)

            max_distance = 0
            camera_movement_x, camera_movement_y = 0,0

            for i, (new,old) in enumerate(zip(new_features,old_features)):
                new_features_point = new.ravel()
                old_features_point = old.ravel()

                distance = measure_distance(new_features_point,old_features_point)
                if distance>max_distance:
                    max_distance = distance
                    camera_movement_x,camera_movement_y = measure_xy_distance(old_features_point, new_features_point ) 
            
            if max_distance > self.minimum_distance:
                camera_movement[frame_num] = [camera_movement_x,camera_movement_y]
                old_features = cv2.goodFeaturesToTrack(frame_gray,**self.features)

            old_gray = frame_gray.copy()
        
        if stub_path is not None:
            with open(stub_path,'wb') as f:
                pickle.dump(camera_movement,f)

        return camera_movement
    
    def draw_camera_movement(self,frames, camera_movement_per_frame):
        output_frames=[]

        for frame_num, frame in enumerate(frames):
            frame= frame.copy()

            overlay = frame.copy()
            cv2.rectangle(overlay,(0,0),(500,100),(255,255,255),-1)
            alpha =0.6
            cv2.addWeighted(overlay,alpha,frame,1-alpha,0,frame)

            x_movement, y_movement = camera_movement_per_frame[frame_num]
            frame = cv2.putText(frame,f"Camera Movement X: {x_movement:.2f}",(10,30), cv2.FONT_HERSHEY_SIMPLEX,1,(0,0,0),3)
            frame = cv2.putText(frame,f"Camera Movement Y: {y_movement:.2f}",(10,60), cv2.FONT_HERSHEY_SIMPLEX,1,(0,0,0),3)

            output_frames.append(frame) 

        return output_frames