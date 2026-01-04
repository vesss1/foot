"""
===============================================================================
SPEED AND DISTANCE ESTIMATOR MODULE
===============================================================================

This module calculates player speed and distance traveled using transformed
field coordinates. It's the final step in converting pixel-based tracking
data into meaningful real-world performance metrics.

METRICS CALCULATED:
1. Speed: Instantaneous speed in km/h for each player per frame
2. Distance: Cumulative distance traveled in meters for each player

CALCULATION METHOD:
- Uses frame windows (5 frames) to reduce noise
- Measures distance between positions in transformed coordinates (meters)
- Divides by elapsed time to get speed
- Accumulates distance across all frames

FRAME WINDOW:
Using a 5-frame window (instead of frame-to-frame) provides more stable
speed measurements by averaging over short time periods and reducing
impact of tracking jitter.

CONVERSIONS:
- Frame rate: 24 fps (assumed)
- Speed: meters/second → km/h (multiply by 3.6)
- Distance: accumulated in meters

OUTPUT:
Both metrics are added to the tracking dictionary and displayed as
annotations on the video output.
===============================================================================
"""

import cv2
import sys 
sys.path.append('../')
from utils import measure_distance ,get_foot_position


################################################################################
# SPEED AND DISTANCE ESTIMATOR CLASS
################################################################################

class SpeedAndDistance_Estimator():
    """
    Calculates player speed and distance from transformed coordinates.
    """
    
    def __init__(self):
        """
        Initialize estimator with frame window and frame rate settings.
        """
        self.frame_window=5   # Number of frames for speed calculation window
        self.frame_rate=24    # Assumed video frame rate (fps)
    
    # =========================================================================
    # METRIC CALCULATION
    # =========================================================================
    
    def add_speed_and_distance_to_tracks(self,tracks):
        """
        Calculate and add speed and distance metrics to all player tracks.
        
        PROCESS:
        1. Skip ball and referees (only calculate for players)
        2. Process frames in windows of 5 frames
        3. For each player in window:
           - Get start and end positions (transformed coordinates in meters)
           - Calculate distance traveled using Euclidean distance
           - Calculate time elapsed based on frame rate
           - Compute speed = distance / time
           - Accumulate total distance
        4. Assign speed and distance to all frames in the window
        
        NOTES:
        - Only calculates for players with valid transformed positions
        - Skips players that disappear mid-window (lost tracking)
        - Speed is constant within each 5-frame window
        
        Args:
            tracks: Complete tracking dictionary for all objects
        """
        total_distance= {}

        for object, object_tracks in tracks.items():
            if object == "ball" or object == "referees":
                continue 
            number_of_frames = len(object_tracks)
            for frame_num in range(0,number_of_frames, self.frame_window):
                last_frame = min(frame_num+self.frame_window,number_of_frames-1 )

                for track_id,_ in object_tracks[frame_num].items():
                    if track_id not in object_tracks[last_frame]:
                        continue

                    start_position = object_tracks[frame_num][track_id]['position_transformed']
                    end_position = object_tracks[last_frame][track_id]['position_transformed']

                    if start_position is None or end_position is None:
                        continue
                    
                    distance_covered = measure_distance(start_position,end_position)
                    time_elapsed = (last_frame-frame_num)/self.frame_rate
                    speed_meteres_per_second = distance_covered/time_elapsed
                    speed_km_per_hour = speed_meteres_per_second*3.6

                    if object not in total_distance:
                        total_distance[object]= {}
                    
                    if track_id not in total_distance[object]:
                        total_distance[object][track_id] = 0
                    
                    total_distance[object][track_id] += distance_covered

                    for frame_num_batch in range(frame_num, last_frame + 1):
                        if track_id not in tracks[object][frame_num_batch]:
                            continue
                        tracks[object][frame_num_batch][track_id]['speed'] = speed_km_per_hour
                        tracks[object][frame_num_batch][track_id]['distance'] = total_distance[object][track_id]
    
    # =========================================================================
    # VISUALIZATION
    # =========================================================================
    
    def draw_speed_and_distance(self,frames,tracks):
        """
        Annotate video frames with speed and distance information.
        
        Draws text overlays showing:
        - Current speed in km/h
        - Total distance traveled in meters
        
        Text is positioned near player's feet for clear association.
        
        Args:
            frames: List of video frames
            tracks: Tracking dictionary with speed/distance data
            
        Returns:
            List of annotated frames
        """
        output_frames = []
        for frame_num, frame in enumerate(frames):
            for object, object_tracks in tracks.items():
                if object == "ball" or object == "referees":
                    continue 
                for _, track_info in object_tracks[frame_num].items():
                   if "speed" in track_info:
                       speed = track_info.get('speed',None)
                       distance = track_info.get('distance',None)
                       if speed is None or distance is None:
                           continue
                       
                       bbox = track_info['bbox']
                       position = get_foot_position(bbox)
                       position = list(position)
                       position[1]+=40

                       position = tuple(map(int,position))
                       cv2.putText(frame, f"{speed:.2f} km/h",position,cv2.FONT_HERSHEY_SIMPLEX,0.5,(0,0,0),2)
                       cv2.putText(frame, f"{distance:.2f} m",(position[0],position[1]+20),cv2.FONT_HERSHEY_SIMPLEX,0.5,(0,0,0),2)
            output_frames.append(frame)
        
        return output_frames