#!/usr/bin/env python3
"""
Football Analysis Qt UI Application (Using .ui file)
Demonstrates how to use the Qt Creator .ui file with PyQt5
"""

import sys
import json
import os

from PyQt5 import uic
from PyQt5.QtWidgets import QApplication, QMainWindow, QFileDialog, QMessageBox
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QPixmap, QImage
import cv2
import numpy as np


class FootballAnalysisUI(QMainWindow):
    """Main window for Football Analysis UI (loaded from .ui file)"""
    
    def __init__(self, ui_file='football_analysis_ui.ui'):
        super().__init__()
        
        # Load the .ui file
        if os.path.exists(ui_file):
            uic.loadUi(ui_file, self)
        else:
            raise FileNotFoundError(f"UI file not found: {ui_file}")
        
        # Paths
        self.output_dir = "output_videos"
        self.video_path = None
        self.data_path = None
        self.stats_data = None
        
        # Video playback using OpenCV
        self.video_capture = None
        self.is_playing = False
        self.current_frame_idx = 0
        self.total_frames = 0
        self.fps = 30
        self.frames = []
        
        # Timer for video playback
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_frame)
        
        # Connect signals from UI elements
        self.connect_signals()
        
        # Auto-load if files exist
        self.auto_load_files()
    
    def connect_signals(self):
        """Connect UI element signals to slots"""
        # Video controls
        self.playButton.clicked.connect(self.toggle_play)
        self.stopButton.clicked.connect(self.stop_video)
        self.loadVideoButton.clicked.connect(self.load_video_file)
        self.positionSlider.sliderMoved.connect(self.seek_to_frame)
        self.positionSlider.sliderPressed.connect(self.slider_pressed)
        self.positionSlider.sliderReleased.connect(self.slider_released)
        
        # Statistics
        self.refreshButton.clicked.connect(self.load_statistics)
    
    def auto_load_files(self):
        """Automatically load video and data files if they exist"""
        video_file = os.path.join(self.output_dir, "output_video.avi")
        data_file = os.path.join(self.output_dir, "data_output.json")
        
        if os.path.exists(video_file):
            self.load_video(video_file)
        
        if os.path.exists(data_file):
            self.load_statistics(data_file)
    
    def load_video_file(self):
        """Open file dialog to load a video"""
        file_path, _ = QFileDialog.getOpenFileName(
            self,
            "Select Video File",
            self.output_dir,
            "Video Files (*.avi *.mp4 *.mov);;All Files (*)"
        )
        
        if file_path:
            self.load_video(file_path)
    
    def load_video(self, file_path):
        """Load a video file using OpenCV"""
        if not os.path.exists(file_path):
            QMessageBox.warning(self, "Error", f"Video file not found: {file_path}")
            return
        
        self.video_path = file_path
        
        # Open video with OpenCV
        self.video_capture = cv2.VideoCapture(file_path)
        
        if not self.video_capture.isOpened():
            QMessageBox.warning(self, "Error", "Failed to open video file")
            return
        
        # Get video properties
        self.fps = self.video_capture.get(cv2.CAP_PROP_FPS)
        
        # Validate FPS to prevent division by zero
        if self.fps <= 0 or self.fps > 300:  # Sanity check
            self.fps = 30  # Default to 30 FPS
            
        self.total_frames = int(self.video_capture.get(cv2.CAP_PROP_FRAME_COUNT))
        
        # Load all frames into memory for smoother playback
        # Note: For very large videos (>5 minutes), consider implementing
        # a frame buffer with lazy loading to reduce memory usage
        self.frames = []
        frame_count = 0
        max_frames = 18000  # Limit to ~10 minutes at 30fps to prevent memory issues
        
        while frame_count < max_frames:
            ret, frame = self.video_capture.read()
            if not ret:
                break
            # Convert BGR to RGB for Qt
            frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            self.frames.append(frame_rgb)
            frame_count += 1
        
        self.video_capture.release()
        self.total_frames = len(self.frames)
        
        if self.total_frames == 0:
            QMessageBox.warning(self, "Error", "No frames found in video")
            return
        
        # Warn if video was truncated
        if frame_count >= max_frames:
            QMessageBox.information(
                self, 
                "Video Truncated", 
                f"Video exceeds maximum length. Only first {max_frames} frames loaded.\n"
                f"For longer videos, consider splitting the analysis."
            )
        
        # Reset to first frame
        self.current_frame_idx = 0
        self.display_current_frame()
        
        # Update UI
        self.playButton.setEnabled(True)
        self.stopButton.setEnabled(True)
        self.positionSlider.setRange(0, self.total_frames - 1)
        self.positionSlider.setValue(0)
        
        # Update video duration label
        if self.fps > 0:
            duration = self.total_frames / self.fps
            self.videoDurationLabel.setText(f"{duration:.1f} s")
    
    def display_current_frame(self):
        """Display the current frame"""
        if not self.frames or self.current_frame_idx >= len(self.frames):
            return
        
        frame = self.frames[self.current_frame_idx]
        
        # Convert numpy array to QImage
        height, width, channel = frame.shape
        bytes_per_line = 3 * width
        q_image = QImage(frame.data, width, height, bytes_per_line, QImage.Format_RGB888)
        
        # Scale to fit label while maintaining aspect ratio
        pixmap = QPixmap.fromImage(q_image)
        scaled_pixmap = pixmap.scaled(
            self.videoLabel.size(),
            Qt.KeepAspectRatio,
            Qt.SmoothTransformation
        )
        
        self.videoLabel.setPixmap(scaled_pixmap)
        
        # Update frame info
        if self.fps > 0:
            current_time = self.current_frame_idx / self.fps
            total_time = self.total_frames / self.fps
            self.frameInfoLabel.setText(
                f"Frame {self.current_frame_idx + 1}/{self.total_frames} | "
                f"Time: {current_time:.1f}s / {total_time:.1f}s"
            )
        else:
            self.frameInfoLabel.setText(
                f"Frame {self.current_frame_idx + 1}/{self.total_frames}"
            )
        
        # Update slider if not being dragged
        if not self.positionSlider.isSliderDown():
            self.positionSlider.setValue(self.current_frame_idx)
    
    def update_frame(self):
        """Update to next frame during playback"""
        if not self.frames:
            return
        
        self.current_frame_idx += 1
        
        if self.current_frame_idx >= self.total_frames:
            # Reached end of video
            self.stop_video()
            return
        
        self.display_current_frame()
    
    def toggle_play(self):
        """Toggle play/pause"""
        if self.is_playing:
            self.pause_video()
        else:
            self.play_video()
    
    def play_video(self):
        """Start video playback"""
        if not self.frames:
            return
        
        self.is_playing = True
        self.playButton.setText("Pause")
        
        # Calculate timer interval based on FPS
        interval = int(1000 / self.fps) if self.fps > 0 else 33  # Default 30 FPS
        self.timer.start(interval)
    
    def pause_video(self):
        """Pause video playback"""
        self.is_playing = False
        self.playButton.setText("Play")
        self.timer.stop()
    
    def stop_video(self):
        """Stop video playback and return to start"""
        self.is_playing = False
        self.playButton.setText("Play")
        self.timer.stop()
        
        if self.frames:
            self.current_frame_idx = 0
            self.display_current_frame()
    
    def seek_to_frame(self, frame_idx):
        """Seek to a specific frame"""
        if not self.frames:
            return
        
        self.current_frame_idx = max(0, min(frame_idx, self.total_frames - 1))
        self.display_current_frame()
    
    def slider_pressed(self):
        """Handle slider press - pause video if playing"""
        if self.is_playing:
            self.timer.stop()
    
    def slider_released(self):
        """Handle slider release - resume video if it was playing"""
        if self.is_playing:
            interval = int(1000 / self.fps) if self.fps > 0 else 33
            self.timer.start(interval)
    
    def load_statistics(self, file_path=None):
        """Load statistics from JSON file"""
        if file_path is None:
            file_path = os.path.join(self.output_dir, "data_output.json")
        
        if not os.path.exists(file_path):
            QMessageBox.information(
                self,
                "No Data",
                "Statistics file not found. Please run the analysis first."
            )
            return
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                self.stats_data = json.load(f)
            
            self.update_statistics_display()
            
        except (FileNotFoundError, json.JSONDecodeError, ValueError, KeyError) as e:
            QMessageBox.warning(
                self,
                "Error",
                f"Failed to load statistics: {str(e)}"
            )
    
    def update_statistics_display(self):
        """Update the statistics display with loaded data"""
        if not self.stats_data:
            return
        
        summary = self.stats_data.get('summary', {})
        
        # Update possession percentages
        team1_possession = summary.get('team_1_possession_percent', 0)
        team2_possession = summary.get('team_2_possession_percent', 0)
        
        self.team1PossessionLabel.setText(f"{team1_possession:.1f}%")
        self.team2PossessionLabel.setText(f"{team2_possession:.1f}%")
        
        # Calculate total distance and average
        total_distance = 0
        player_count = 0
        
        for key in self.stats_data:
            if key.startswith('team_'):
                team_data = self.stats_data[key]
                if isinstance(team_data, dict):
                    for player_id, player_data in team_data.items():
                        if isinstance(player_data, dict):
                            distance_km = player_data.get('distance_km', 0)
                            total_distance += distance_km
                            player_count += 1
        
        self.totalDistanceLabel.setText(f"{total_distance:.2f} km")
        
        if player_count > 0:
            avg_distance = total_distance / player_count
            self.avgDistanceLabel.setText(f"{avg_distance:.2f} km")
            
            # Calculate average speed (if we have video duration)
            duration_text = self.videoDurationLabel.text()
            if duration_text != "--- s":
                try:
                    duration_sec = float(duration_text.split()[0])
                    duration_hours = duration_sec / 3600
                    if duration_hours > 0:
                        avg_speed = avg_distance / duration_hours
                        self.avgSpeedLabel.setText(f"{avg_speed:.2f} km/h")
                except (ValueError, IndexError, ZeroDivisionError):
                    # Invalid duration format or calculation error
                    pass


def main():
    """Main entry point"""
    app = QApplication(sys.argv)
    
    # Set application style
    app.setStyle('Fusion')
    
    # Create and show main window
    window = FootballAnalysisUI()
    window.show()
    
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
