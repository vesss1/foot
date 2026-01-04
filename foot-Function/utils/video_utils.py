"""
===============================================================================
VIDEO I/O UTILITIES
===============================================================================

This module handles video input/output operations with comprehensive
error handling and validation.

FUNCTIONS:
- read_video: Load video frames from file into memory
- save_video: Write processed frames to video file

ERROR HANDLING:
Both functions include extensive validation and error checking to ensure
robust operation in production environments. They handle:
- File existence and accessibility
- Invalid video formats or corrupted files
- Empty frame sequences
- Inconsistent frame dimensions
- Codec compatibility issues
- Disk space and write permissions

USAGE:
Called by main pipeline to load input video and save annotated output.
All errors are raised as exceptions with descriptive messages for debugging.
===============================================================================
"""

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
    
    # Initialize VideoWriter with validated parameters
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    fps = 24
    
    logger.info(f"Initializing VideoWriter with codec=XVID, fps={fps}, size=({width}, {height})")
    
    out = cv2.VideoWriter(
        output_video_path,
        fourcc,
        fps,
        (width, height)
    )
    
    # Critical check: VideoWriter must be opened before writing
    if not out.isOpened():
        out.release()
        raise IOError(
            f"Failed to open VideoWriter for {output_video_path}. "
            f"Codec 'XVID' may not be supported. "
            f"Frame size: {width}x{height}, FPS: {fps}"
        )
    
    logger.info("VideoWriter opened successfully")
    
    # Write frames
    frames_written = 0
    frames_skipped = 0
    
    try:
        for i, frame in enumerate(output_video_frames):
            if frame is None:
                logger.warning(f"Skipping None frame at index {i}")
                frames_skipped += 1
                continue
            
            # Validate frame has correct shape
            if len(frame.shape) < 2:
                logger.warning(f"Skipping frame {i}: invalid shape {frame.shape}")
                frames_skipped += 1
                continue
                
            # Validate frame dimensions match
            frame_height, frame_width = frame.shape[:2]
            if (frame_height, frame_width) != (height, width):
                logger.warning(
                    f"Frame {i} has different dimensions ({frame_width}x{frame_height}), "
                    f"expected ({width}x{height}). Resizing..."
                )
                frame = cv2.resize(frame, (width, height))
            
            # Write frame - cv2.VideoWriter.write() returns None, not boolean
            # We need to check if writer is still open instead
            if not out.isOpened():
                logger.error(f"VideoWriter closed unexpectedly at frame {i}")
                break
            
            out.write(frame)
            frames_written += 1
                
    except Exception as e:
        logger.error(f"Error writing frames to video: {e}")
        raise IOError(f"Error writing frames to video: {e}")
    finally:
        out.release()
        logger.info(f"VideoWriter released. Frames written: {frames_written}, skipped: {frames_skipped}")
    
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

