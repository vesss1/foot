import cv2
import os
import logging

logger = logging.getLogger(__name__)


def read_video(video_path):
    """
    Read video frames from file with error handling.
    
    Args:
        video_path: Path to video file
        
    Returns:
        List of frames as numpy arrays
        
    Raises:
        FileNotFoundError: If video file doesn't exist
        IOError: If video cannot be opened or read
    """
    if not os.path.exists(video_path):
        raise FileNotFoundError(f"Video file not found: {video_path}")
    
    cap = cv2.VideoCapture(video_path)
    
    if not cap.isOpened():
        raise IOError(f"Cannot open video file: {video_path}")
    
    frames = []
    frame_count = 0
    
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        frames.append(frame)
        frame_count += 1
    
    cap.release()
    
    if not frames:
        raise IOError(f"No frames read from video: {video_path}")
    
    logger.info(f"Read {frame_count} frames from {video_path}")
    
    return frames


def save_video(output_video_frames, output_video_path):
    """
    Save video frames to file with robust error handling.
    
    Args:
        output_video_frames: List of frames to save
        output_video_path: Path where video should be saved
        
    Raises:
        ValueError: If frames list is empty or invalid
        IOError: If video cannot be written or output directory doesn't exist
    """
    # Validate inputs
    if not output_video_frames:
        raise ValueError("Cannot save video: frames list is empty")
    
    if not isinstance(output_video_frames, list):
        raise TypeError("output_video_frames must be a list")
    
    # Validate output path
    if not output_video_path or not output_video_path.strip():
        raise ValueError("Output video path cannot be empty")
    
    # Create output directory if needed
    output_dir = os.path.dirname(output_video_path)
    if output_dir and not os.path.exists(output_dir):
        try:
            os.makedirs(output_dir, exist_ok=True)
            logger.info(f"Created output directory: {output_dir}")
        except Exception as e:
            raise IOError(f"Failed to create output directory {output_dir}: {e}")
    
    # Get video properties from first frame
    try:
        first_frame = output_video_frames[0]
        height, width = first_frame.shape[:2]
        
        if height <= 0 or width <= 0:
            raise ValueError(f"Invalid frame dimensions: {width}x{height}")
        
        logger.info(f"Video dimensions: {width}x{height}, frames: {len(output_video_frames)}")
        
    except (IndexError, AttributeError) as e:
        raise ValueError(f"Invalid frame data: {e}")
    
    # Initialize VideoWriter
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    fps = 24
    
    out = cv2.VideoWriter(
        output_video_path,
        fourcc,
        fps,
        (width, height)
    )
    
    if not out.isOpened():
        raise IOError(
            f"Failed to create VideoWriter for {output_video_path}. "
            f"Check that output path is valid and codec is supported."
        )
    
    # Write frames
    try:
        for i, frame in enumerate(output_video_frames):
            if frame is None:
                logger.warning(f"Skipping None frame at index {i}")
                continue
                
            # Validate frame dimensions match
            if frame.shape[:2] != (height, width):
                logger.warning(
                    f"Frame {i} has different dimensions {frame.shape[:2]}, "
                    f"expected ({height}, {width}). Resizing..."
                )
                frame = cv2.resize(frame, (width, height))
            
            success = out.write(frame)
            if not success:
                logger.warning(f"Failed to write frame {i}")
                
    except Exception as e:
        raise IOError(f"Error writing frames to video: {e}")
    finally:
        out.release()
    
    # Verify output file was created
    if not os.path.exists(output_video_path):
        raise IOError(f"Output video file was not created: {output_video_path}")
    
    file_size = os.path.getsize(output_video_path)
    if file_size == 0:
        raise IOError(f"Output video file is empty: {output_video_path}")
    
    logger.info(
        f"Video saved successfully: {output_video_path} "
        f"({file_size / 1024 / 1024:.2f} MB)"
    )

