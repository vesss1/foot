"""
===============================================================================
PLAYER-BALL ASSIGNER MODULE
===============================================================================

This module determines ball possession by finding which player is closest
to the ball in each frame.

POSSESSION LOGIC:
- Calculate distance from ball center to left and right feet of each player
- Use minimum distance (ball may be to left or right side of player)
- Assign possession to player with minimum distance < threshold
- If no player is within threshold, ball is unassigned

THRESHOLD:
max_player_ball_distance = 70 pixels
This empirical value balances false positives (assigning too easily) and
false negatives (missing actual possession).

USAGE:
Called for each frame to determine which player has the ball.
Results are used to:
- Mark possessed player with red triangle annotation
- Calculate team possession statistics
===============================================================================
"""

import sys 
sys.path.append('../')
from utils import get_center_of_bbox, measure_distance


################################################################################
# PLAYER BALL ASSIGNER CLASS
################################################################################

class PlayerBallAssigner():
    """
    Assigns ball possession to the nearest player within distance threshold.
    """
    
    def __init__(self):
        """Initialize with maximum distance threshold for ball assignment."""
        self.max_player_ball_distance = 70  # Maximum distance in pixels for possession
    
    def assign_ball_to_player(self,players,ball_bbox):
        """
        Determine which player has possession of the ball.
        
        ALGORITHM:
        1. Get ball position (center of bbox)
        2. For each player:
           - Calculate distance from ball to player's left foot
           - Calculate distance from ball to player's right foot
           - Take minimum (ball could be on either side)
        3. Find player with smallest distance
        4. Assign ball if distance < threshold, otherwise return -1
        
        Args:
            players: Dictionary of players in current frame {player_id: {bbox: [...]}}
            ball_bbox: Ball bounding box [x1, y1, x2, y2]
            
        Returns:
            player_id of player with ball, or -1 if no player is close enough
        """
        ball_position = get_center_of_bbox(ball_bbox)

        miniumum_distance = 99999
        assigned_player=-1

        for player_id, player in players.items():
            player_bbox = player['bbox']

            distance_left = measure_distance((player_bbox[0],player_bbox[-1]),ball_position)
            distance_right = measure_distance((player_bbox[2],player_bbox[-1]),ball_position)
            distance = min(distance_left,distance_right)

            if distance < self.max_player_ball_distance:
                if distance < miniumum_distance:
                    miniumum_distance = distance
                    assigned_player = player_id

        return assigned_player