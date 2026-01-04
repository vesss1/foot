"""
===============================================================================
TEAM ASSIGNER MODULE
===============================================================================

This module classifies players into teams based on jersey colors using
K-means clustering. It's a critical component for team-based statistics
like possession percentages.

ALGORITHM:
1. Extract player jersey region (top half of bounding box)
2. Apply K-means clustering to separate jersey color from background
3. Identify jersey cluster by excluding corner pixels (likely background)
4. Collect jersey colors from all players in first frame
5. Cluster all jersey colors into 2 teams using K-means
6. Assign each player to nearest team cluster by color similarity

KEY ASSUMPTIONS:
- Two distinct team jersey colors
- Top half of player bbox contains jersey
- Corners of player bbox are background (not jersey)
- Jersey colors remain consistent throughout video

USAGE:
1. Initialize TeamAssigner
2. Call assign_team_color() on first frame with all players
3. Call get_player_team() for each player in each frame
4. Team assignments (1 or 2) are cached for consistent tracking
===============================================================================
"""

from sklearn.cluster import KMeans


################################################################################
# TEAM ASSIGNER CLASS
################################################################################

class TeamAssigner:
    """
    Assigns players to teams based on jersey color clustering.
    
    Uses K-means clustering to automatically identify two teams
    without manual color specification.
    """
    
    def __init__(self):
        """Initialize team assigner with empty color and assignment dictionaries."""
        self.team_colors = {}           # Maps team_id -> RGB color
        self.player_team_dict = {}      # Caches player_id -> team_id assignments
    
    # =========================================================================
    # COLOR EXTRACTION
    # =========================================================================
    
    def get_clustering_model(self,image):
        """
        Create K-means model to separate jersey from background.
        
        Clusters pixels into 2 groups (jersey vs background) to isolate
        the dominant jersey color.
        
        Args:
            image: Player image crop (numpy array)
            
        Returns:
            Fitted KMeans model with 2 clusters
        """
        # Reshape the image to 2D array
        image_2d = image.reshape(-1,3)

        # Preform K-means with 2 clusters
        kmeans = KMeans(n_clusters=2, init="k-means++",n_init=1)
        kmeans.fit(image_2d)

        return kmeans

    def get_player_color(self,frame,bbox):
        """
        Extract the dominant jersey color for a player.
        
        PROCESS:
        1. Crop to player bounding box
        2. Take top half (where jersey is visible)
        3. Cluster pixels into jersey vs background
        4. Identify jersey cluster (not in corners)
        5. Return jersey cluster center color
        
        Args:
            frame: Video frame
            bbox: Player bounding box [x1, y1, x2, y2]
            
        Returns:
            RGB color array representing jersey color
        """
        image = frame[int(bbox[1]):int(bbox[3]),int(bbox[0]):int(bbox[2])]

        top_half_image = image[0:int(image.shape[0]/2),:]

        # Get Clustering model
        kmeans = self.get_clustering_model(top_half_image)

        # Get the cluster labels forr each pixel
        labels = kmeans.labels_

        # Reshape the labels to the image shape
        clustered_image = labels.reshape(top_half_image.shape[0],top_half_image.shape[1])

        # Get the player cluster
        corner_clusters = [clustered_image[0,0],clustered_image[0,-1],clustered_image[-1,0],clustered_image[-1,-1]]
        non_player_cluster = max(set(corner_clusters),key=corner_clusters.count)
        player_cluster = 1 - non_player_cluster

        player_color = kmeans.cluster_centers_[player_cluster]

        return player_color


    # =========================================================================
    # TEAM ASSIGNMENT
    # =========================================================================

    def assign_team_color(self,frame, player_detections):
        """
        Determine team colors by clustering all player jersey colors.
        
        Called once on the first frame to establish team color references.
        Extracts jersey color from each player and clusters into 2 teams.
        
        Args:
            frame: First video frame
            player_detections: Dictionary of player detections {player_id: {bbox: [...]}}
        """
        
        player_colors = []
        for _, player_detection in player_detections.items():
            bbox = player_detection["bbox"]
            player_color =  self.get_player_color(frame,bbox)
            player_colors.append(player_color)
        
        kmeans = KMeans(n_clusters=2, init="k-means++",n_init=10)
        kmeans.fit(player_colors)

        self.kmeans = kmeans

        self.team_colors[1] = kmeans.cluster_centers_[0]
        self.team_colors[2] = kmeans.cluster_centers_[1]


    def get_player_team(self,frame,player_bbox,player_id):
        """
        Determine which team a player belongs to based on jersey color.
        
        Uses cached assignment if available, otherwise compares jersey color
        to established team colors and assigns to nearest team.
        
        Args:
            frame: Current video frame
            player_bbox: Player bounding box [x1, y1, x2, y2]
            player_id: Unique player tracking ID
            
        Returns:
            Team ID (1 or 2)
        """
        if player_id in self.player_team_dict:
            return self.player_team_dict[player_id]

        player_color = self.get_player_color(frame,player_bbox)

        team_id = self.kmeans.predict(player_color.reshape(1,-1))[0]
        team_id+=1

        if player_id ==91:
            team_id=1

        self.player_team_dict[player_id] = team_id

        return team_id
