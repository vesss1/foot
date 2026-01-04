"""
===============================================================================
VIEW TRANSFORMER MODULE
===============================================================================

This module performs perspective transformation to convert 2D pixel coordinates
from the video into real-world 2D coordinates on the football field.

PROBLEM:
Video captures the field from an angled perspective. A player moving 10 pixels
near the camera covers less real-world distance than 10 pixels far from camera.
Direct pixel measurements would be inaccurate for speed and distance.

SOLUTION:
Use perspective transformation (homography) to map video coordinates to a
top-down field view where distances can be measured accurately in meters.

TRANSFORMATION SETUP:
1. Define 4 reference points in video (pixel_vertices)
2. Define corresponding points in real field coordinates (target_vertices)
3. Compute perspective transformation matrix
4. Apply transformation to all object positions

COORDINATE SYSTEM:
- Pixel coordinates: Video frame coordinates
- Field coordinates: Top-down view in meters
  - Origin: Top-left corner of reference area
  - X-axis: Along field length (23.32 meters)
  - Y-axis: Across field width (68 meters)

REFERENCE POINTS:
The 4 points define a quadrilateral on the field that's visible in video.
These are manually calibrated based on the specific camera angle and position.
===============================================================================
"""

import numpy as np 
import cv2


################################################################################
# VIEW TRANSFORMER CLASS
################################################################################

class ViewTransformer():
    """
    Transforms pixel coordinates to real-world field coordinates.
    
    Uses perspective transformation to enable accurate distance measurements.
    """
    
    def __init__(self):
        """
        Initialize transformer with field dimensions and reference points.
        
        The reference points map a visible quadrilateral in the video to
        real-world field coordinates in meters.
        """
        # Real-world field dimensions (meters)
        court_width = 68      # Field width
        court_length = 23.32  # Field length (for reference area)

        # Four reference points in video (pixel coordinates)
        # These define a quadrilateral visible in the video
        self.pixel_vertices = np.array([[110, 1035],   # Bottom-left
                               [265, 275],             # Top-left
                               [910, 260],             # Top-right
                               [1640, 915]])           # Bottom-right
        
        # Corresponding points in real field coordinates (meters)
        self.target_vertices = np.array([
            [0,court_width],        # Bottom-left corner
            [0, 0],                 # Top-left corner
            [court_length, 0],      # Top-right corner
            [court_length, court_width]  # Bottom-right corner
        ])

        # Convert to float32 for OpenCV
        self.pixel_vertices = self.pixel_vertices.astype(np.float32)
        self.target_vertices = self.target_vertices.astype(np.float32)

        # Compute perspective transformation matrix
        self.persepctive_trasnformer = cv2.getPerspectiveTransform(self.pixel_vertices, self.target_vertices)

    # =========================================================================
    # COORDINATE TRANSFORMATION
    # =========================================================================

    def transform_point(self,point):
        """
        Transform a single point from pixel to field coordinates.
        
        PROCESS:
        1. Check if point is inside reference quadrilateral
        2. If outside, return None (cannot transform accurately)
        3. If inside, apply perspective transformation
        4. Return transformed coordinates in meters
        
        Args:
            point: [x, y] point in pixel coordinates
            
        Returns:
            [x, y] point in field coordinates (meters), or None if outside bounds
        """
        p = (int(point[0]),int(point[1]))
        is_inside = cv2.pointPolygonTest(self.pixel_vertices,p,False) >= 0 
        if not is_inside:
            return None

        reshaped_point = point.reshape(-1,1,2).astype(np.float32)
        tranform_point = cv2.perspectiveTransform(reshaped_point,self.persepctive_trasnformer)
        return tranform_point.reshape(-1,2)

    def add_transformed_position_to_tracks(self,tracks):
        """
        Apply perspective transformation to all tracked object positions.
        
        Transforms camera-adjusted pixel positions to real-world field
        coordinates for all objects across all frames.
        
        Positions outside the reference area are set to None (cannot transform).
        
        Args:
            tracks: Tracking dictionary for all objects
        """
        for object, object_tracks in tracks.items():
            for frame_num, track in enumerate(object_tracks):
                for track_id, track_info in track.items():
                    position = track_info['position_adjusted']
                    position = np.array(position)
                    position_trasnformed = self.transform_point(position)
                    if position_trasnformed is not None:
                        position_trasnformed = position_trasnformed.squeeze().tolist()
                    tracks[object][frame_num][track_id]['position_transformed'] = position_trasnformed