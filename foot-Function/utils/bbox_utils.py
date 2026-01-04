"""
===============================================================================
BOUNDING BOX UTILITIES
===============================================================================

This module provides utility functions for working with bounding boxes
in the football video analysis system.

BOUNDING BOX FORMAT:
All functions expect bounding boxes as [x1, y1, x2, y2] where:
- (x1, y1): Top-left corner
- (x2, y2): Bottom-right corner

FUNCTIONS:
- get_center_of_bbox: Calculate center point (used for ball position)
- get_bbox_width: Calculate width (used for ellipse scaling)
- measure_distance: Euclidean distance between two points
- measure_xy_distance: Component-wise distance (used for camera movement)
- get_foot_position: Bottom-center point (used for player position on ground)

USAGE:
These utilities are used throughout the analysis pipeline for:
- Converting bounding boxes to ground positions
- Measuring distances for speed calculations
- Drawing annotations at appropriate locations
===============================================================================
"""

def get_center_of_bbox(bbox):
    """
    Calculate the center point of a bounding box.
    
    Used primarily for ball position (center of detection).
    
    Args:
        bbox: [x1, y1, x2, y2] bounding box coordinates
        
    Returns:
        (x, y) tuple of center coordinates as integers
    """
    x1,y1,x2,y2 = bbox
    return int((x1+x2)/2),int((y1+y2)/2)

def get_bbox_width(bbox):
    """
    Calculate the width of a bounding box.
    
    Used for scaling ellipse size to match player bounding box width.
    
    Args:
        bbox: [x1, y1, x2, y2] bounding box coordinates
        
    Returns:
        Width in pixels
    """
    return bbox[2]-bbox[0]

def measure_distance(p1,p2):
    """
    Calculate Euclidean distance between two points.
    
    Used for speed and distance calculations in the analysis pipeline.
    
    Args:
        p1: (x, y) first point
        p2: (x, y) second point
        
    Returns:
        Distance as float
    """
    return ((p1[0]-p2[0])**2 + (p1[1]-p2[1])**2)**0.5

def measure_xy_distance(p1,p2):
    """
    Calculate component-wise distance between two points.
    
    Used for camera movement estimation (separate x and y components).
    
    Args:
        p1: (x, y) first point
        p2: (x, y) second point
        
    Returns:
        (dx, dy) tuple of x and y distances
    """
    return p1[0]-p2[0],p1[1]-p2[1]

def get_foot_position(bbox):
    """
    Calculate the bottom-center point of a bounding box.
    
    This represents where the player's feet contact the ground,
    which is used for accurate position tracking and distance measurement.
    
    Args:
        bbox: [x1, y1, x2, y2] bounding box coordinates
        
    Returns:
        (x, y) tuple of foot position as integers
    """
    x1,y1,x2,y2 = bbox
    return int((x1+x2)/2),int(y2)