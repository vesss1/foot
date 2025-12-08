"""
Football Analysis Qt GUI Application

This Qt-based GUI application provides an interactive interface for the football
analysis system, allowing users to:
- Load and analyze video files
- Configure analysis parameters
- View real-time processing progress
- Play back analyzed videos
- Export results
"""

import sys
import os
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QPushButton, QLabel, QFileDialog, 
                             QProgressBar, QGroupBox, QCheckBox, QSpinBox,
                             QTextEdit, QSplitter, QSlider, QComboBox)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QTimer
from PyQt5.QtGui import QImage, QPixmap, QFont
import cv2
import numpy as np

# Import existing analysis modules
from utils import read_video, save_video
from trackers import Tracker
from team_assigner import TeamAssigner
from player_ball_assigner import PlayerBallAssigner
from camera_movement_estimator import CameraMovementEstimator
from view_transformer import ViewTransformer
from speed_and_distance_estimator import SpeedAndDistance_Estimator


class VideoAnalysisThread(QThread):
    """Background thread for video analysis to keep GUI responsive"""
    
    progress_update = pyqtSignal(int, str)  # progress percentage, status message
    frame_processed = pyqtSignal(int, np.ndarray)  # frame number, processed frame
    analysis_complete = pyqtSignal(str)  # output path
    error_occurred = pyqtSignal(str)  # error message
    
    def __init__(self, video_path, output_path, model_path, config):
        super().__init__()
        self.video_path = video_path
        self.output_path = output_path
        self.model_path = model_path
        self.config = config
        self._is_running = True
        
    def run(self):
        """Execute the video analysis pipeline"""
        try:
            # Read Video
            self.progress_update.emit(5, "Reading video file...")
            video_frames = read_video(self.video_path)
            total_frames = len(video_frames)
            
            if not self._is_running:
                return
            
            # Initialize Tracker
            self.progress_update.emit(10, "Initializing YOLO tracker...")
            tracker = Tracker(self.model_path)
            
            # Get object tracks
            self.progress_update.emit(15, "Detecting and tracking objects...")
            tracks = tracker.get_object_tracks(
                video_frames,
                read_from_stub=self.config.get('use_cache', True),
                stub_path='stubs/track_stubs.pkl'
            )
            
            if not self._is_running:
                return
            
            # Get object positions
            self.progress_update.emit(25, "Calculating object positions...")
            tracker.add_position_to_tracks(tracks)
            
            # Camera movement estimator
            self.progress_update.emit(30, "Estimating camera movement...")
            camera_movement_estimator = CameraMovementEstimator(video_frames[0])
            camera_movement_per_frame = camera_movement_estimator.get_camera_movement(
                video_frames,
                read_from_stub=self.config.get('use_cache', True),
                stub_path='stubs/camera_movement_stub.pkl'
            )
            camera_movement_estimator.add_adjust_positions_to_tracks(tracks, camera_movement_per_frame)
            
            if not self._is_running:
                return
            
            # View Transformer
            self.progress_update.emit(40, "Applying perspective transformation...")
            view_transformer = ViewTransformer()
            view_transformer.add_transformed_position_to_tracks(tracks)
            
            # Interpolate Ball Positions
            self.progress_update.emit(45, "Interpolating ball positions...")
            tracks["ball"] = tracker.interpolate_ball_positions(tracks["ball"])
            
            # Speed and distance estimator
            self.progress_update.emit(50, "Calculating speed and distance...")
            speed_and_distance_estimator = SpeedAndDistance_Estimator()
            speed_and_distance_estimator.add_speed_and_distance_to_tracks(tracks)
            
            if not self._is_running:
                return
            
            # Assign Player Teams
            self.progress_update.emit(55, "Assigning player teams...")
            team_assigner = TeamAssigner()
            team_assigner.assign_team_color(video_frames[0], tracks['players'][0])
            
            for frame_num, player_track in enumerate(tracks['players']):
                for player_id, track in player_track.items():
                    team = team_assigner.get_player_team(
                        video_frames[frame_num],
                        track['bbox'],
                        player_id
                    )
                    tracks['players'][frame_num][player_id]['team'] = team
                    tracks['players'][frame_num][player_id]['team_color'] = team_assigner.team_colors[team]
            
            if not self._is_running:
                return
            
            # Assign Ball Acquisition
            self.progress_update.emit(60, "Analyzing ball possession...")
            player_assigner = PlayerBallAssigner()
            team_ball_control = []
            
            for frame_num, player_track in enumerate(tracks['players']):
                ball_bbox = tracks['ball'][frame_num][1]['bbox']
                assigned_player = player_assigner.assign_ball_to_player(player_track, ball_bbox)
                
                if assigned_player != -1:
                    tracks['players'][frame_num][assigned_player]['has_ball'] = True
                    team_ball_control.append(tracks['players'][frame_num][assigned_player]['team'])
                else:
                    team_ball_control.append(team_ball_control[-1] if team_ball_control else 1)
            
            team_ball_control = np.array(team_ball_control)
            
            if not self._is_running:
                return
            
            # Draw output
            self.progress_update.emit(70, "Generating annotated frames...")
            output_video_frames = tracker.draw_annotations(video_frames, tracks, team_ball_control)
            
            # Draw Camera movement
            output_video_frames = camera_movement_estimator.draw_camera_movement(
                output_video_frames, camera_movement_per_frame
            )
            
            # Draw Speed and Distance
            speed_and_distance_estimator.draw_speed_and_distance(output_video_frames, tracks)
            
            if not self._is_running:
                return
            
            # Save video
            self.progress_update.emit(90, "Saving output video...")
            save_video(output_video_frames, self.output_path)
            
            self.progress_update.emit(100, "Analysis complete!")
            self.analysis_complete.emit(self.output_path)
            
        except Exception as e:
            self.error_occurred.emit(f"Error during analysis: {str(e)}")
    
    def stop(self):
        """Stop the analysis thread"""
        self._is_running = False


class VideoPlayer(QWidget):
    """Widget for displaying video frames"""
    
    def __init__(self):
        super().__init__()
        self.setup_ui()
        self.current_frame = None
        self.frame_list = []
        self.current_frame_idx = 0
        
    def setup_ui(self):
        layout = QVBoxLayout()
        
        # Video display label
        self.video_label = QLabel()
        self.video_label.setAlignment(Qt.AlignCenter)
        self.video_label.setMinimumSize(800, 450)
        self.video_label.setStyleSheet("background-color: black; border: 2px solid #555;")
        self.video_label.setText("No video loaded")
        layout.addWidget(self.video_label)
        
        # Playback controls
        controls_layout = QHBoxLayout()
        
        self.play_button = QPushButton("Play")
        self.play_button.clicked.connect(self.toggle_play)
        self.play_button.setEnabled(False)
        controls_layout.addWidget(self.play_button)
        
        self.frame_slider = QSlider(Qt.Horizontal)
        self.frame_slider.setEnabled(False)
        self.frame_slider.valueChanged.connect(self.slider_changed)
        controls_layout.addWidget(self.frame_slider)
        
        self.frame_label = QLabel("Frame: 0/0")
        controls_layout.addWidget(self.frame_label)
        
        layout.addLayout(controls_layout)
        self.setLayout(layout)
        
        # Timer for playback
        self.timer = QTimer()
        self.timer.timeout.connect(self.next_frame)
        self.is_playing = False
        
    def load_video(self, video_path):
        """Load video frames from file"""
        try:
            self.frame_list = read_video(video_path)
            self.current_frame_idx = 0
            self.frame_slider.setMaximum(len(self.frame_list) - 1)
            self.frame_slider.setEnabled(True)
            self.play_button.setEnabled(True)
            self.display_frame(0)
        except Exception as e:
            print(f"Error loading video: {e}")
    
    def display_frame(self, frame_idx):
        """Display a specific frame"""
        if 0 <= frame_idx < len(self.frame_list):
            self.current_frame_idx = frame_idx
            frame = self.frame_list[frame_idx]
            
            # Convert BGR to RGB
            rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            h, w, ch = rgb_frame.shape
            bytes_per_line = ch * w
            
            # Create QImage and scale to fit
            qt_image = QImage(rgb_frame.data, w, h, bytes_per_line, QImage.Format_RGB888)
            pixmap = QPixmap.fromImage(qt_image)
            scaled_pixmap = pixmap.scaled(self.video_label.size(), Qt.KeepAspectRatio, Qt.SmoothTransformation)
            
            self.video_label.setPixmap(scaled_pixmap)
            self.frame_label.setText(f"Frame: {frame_idx + 1}/{len(self.frame_list)}")
            self.frame_slider.setValue(frame_idx)
    
    def toggle_play(self):
        """Toggle video playback"""
        if self.is_playing:
            self.timer.stop()
            self.play_button.setText("Play")
            self.is_playing = False
        else:
            self.timer.start(42)  # ~24 fps
            self.play_button.setText("Pause")
            self.is_playing = True
    
    def next_frame(self):
        """Display next frame"""
        if self.current_frame_idx < len(self.frame_list) - 1:
            self.display_frame(self.current_frame_idx + 1)
        else:
            self.toggle_play()  # Stop at end
    
    def slider_changed(self, value):
        """Handle slider movement"""
        if not self.is_playing:
            self.display_frame(value)


class FootballAnalysisGUI(QMainWindow):
    """Main application window"""
    
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Football Analysis System - Qt GUI")
        self.setGeometry(100, 100, 1400, 800)
        
        self.video_path = None
        self.output_path = None
        self.model_path = "models/best.pt"
        self.analysis_thread = None
        
        self.setup_ui()
        
    def setup_ui(self):
        """Setup the user interface"""
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        main_layout = QHBoxLayout()
        
        # Left panel: Controls and settings
        left_panel = QWidget()
        left_layout = QVBoxLayout()
        
        # Title
        title = QLabel("Football Analysis System")
        title_font = QFont()
        title_font.setPointSize(16)
        title_font.setBold(True)
        title.setFont(title_font)
        title.setAlignment(Qt.AlignCenter)
        left_layout.addWidget(title)
        
        # File selection group
        file_group = QGroupBox("Video Files")
        file_layout = QVBoxLayout()
        
        input_layout = QHBoxLayout()
        self.input_label = QLabel("No video selected")
        self.input_label.setWordWrap(True)
        input_layout.addWidget(self.input_label)
        select_btn = QPushButton("Browse...")
        select_btn.clicked.connect(self.select_input_video)
        input_layout.addWidget(select_btn)
        file_layout.addLayout(input_layout)
        
        output_layout = QHBoxLayout()
        self.output_label = QLabel("Output: output_videos/output_video.avi")
        self.output_label.setWordWrap(True)
        output_layout.addWidget(self.output_label)
        output_btn = QPushButton("Change...")
        output_btn.clicked.connect(self.select_output_path)
        output_layout.addWidget(output_btn)
        file_layout.addLayout(output_layout)
        
        file_group.setLayout(file_layout)
        left_layout.addWidget(file_group)
        
        # Settings group
        settings_group = QGroupBox("Analysis Settings")
        settings_layout = QVBoxLayout()
        
        self.use_cache_checkbox = QCheckBox("Use cached data (faster)")
        self.use_cache_checkbox.setChecked(True)
        settings_layout.addWidget(self.use_cache_checkbox)
        
        model_layout = QHBoxLayout()
        model_layout.addWidget(QLabel("Model:"))
        self.model_combo = QComboBox()
        self.model_combo.addItems(["models/best.pt", "Custom..."])
        self.model_combo.currentTextChanged.connect(self.model_changed)
        model_layout.addWidget(self.model_combo)
        settings_layout.addLayout(model_layout)
        
        settings_group.setLayout(settings_layout)
        left_layout.addWidget(settings_group)
        
        # Analysis controls
        control_group = QGroupBox("Analysis Control")
        control_layout = QVBoxLayout()
        
        self.analyze_btn = QPushButton("Start Analysis")
        self.analyze_btn.setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 10px; }")
        self.analyze_btn.clicked.connect(self.start_analysis)
        control_layout.addWidget(self.analyze_btn)
        
        self.stop_btn = QPushButton("Stop")
        self.stop_btn.setEnabled(False)
        self.stop_btn.clicked.connect(self.stop_analysis)
        control_layout.addWidget(self.stop_btn)
        
        self.progress_bar = QProgressBar()
        control_layout.addWidget(self.progress_bar)
        
        self.status_label = QLabel("Ready")
        self.status_label.setWordWrap(True)
        control_layout.addWidget(self.status_label)
        
        control_group.setLayout(control_layout)
        left_layout.addWidget(control_group)
        
        # Log output
        log_group = QGroupBox("Analysis Log")
        log_layout = QVBoxLayout()
        
        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setMaximumHeight(200)
        log_layout.addWidget(self.log_text)
        
        log_group.setLayout(log_layout)
        left_layout.addWidget(log_group)
        
        left_layout.addStretch()
        left_panel.setLayout(left_layout)
        left_panel.setMaximumWidth(400)
        
        # Right panel: Video player
        self.video_player = VideoPlayer()
        
        # Add panels to main layout
        main_layout.addWidget(left_panel)
        main_layout.addWidget(self.video_player, 1)
        
        central_widget.setLayout(main_layout)
    
    def select_input_video(self):
        """Open file dialog to select input video"""
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Select Input Video", "input_videos",
            "Video Files (*.mp4 *.avi *.mov);;All Files (*)"
        )
        if file_path:
            self.video_path = file_path
            self.input_label.setText(f"Input: {os.path.basename(file_path)}")
            self.log_message(f"Selected input video: {file_path}")
            
            # Try to load and display first frame
            try:
                self.video_player.load_video(file_path)
                self.log_message(f"Loaded video preview")
            except Exception as e:
                self.log_message(f"Could not load preview: {e}")
    
    def select_output_path(self):
        """Open file dialog to select output path"""
        file_path, _ = QFileDialog.getSaveFileName(
            self, "Select Output Path", "output_videos/output_video.avi",
            "Video Files (*.avi);;All Files (*)"
        )
        if file_path:
            self.output_path = file_path
            self.output_label.setText(f"Output: {os.path.basename(file_path)}")
            self.log_message(f"Output will be saved to: {file_path}")
    
    def model_changed(self, text):
        """Handle model selection change"""
        if text == "Custom...":
            file_path, _ = QFileDialog.getOpenFileName(
                self, "Select YOLO Model", "models",
                "Model Files (*.pt);;All Files (*)"
            )
            if file_path:
                self.model_path = file_path
                self.log_message(f"Selected model: {file_path}")
            else:
                self.model_combo.setCurrentIndex(0)
        else:
            self.model_path = text
    
    def start_analysis(self):
        """Start video analysis in background thread"""
        if not self.video_path:
            self.log_message("ERROR: Please select an input video first!")
            return
        
        if not os.path.exists(self.model_path):
            self.log_message(f"ERROR: Model file not found: {self.model_path}")
            return
        
        # Set output path if not set
        if not self.output_path:
            self.output_path = "output_videos/output_video.avi"
        
        # Ensure output directory exists
        os.makedirs(os.path.dirname(self.output_path), exist_ok=True)
        
        # Configure analysis
        config = {
            'use_cache': self.use_cache_checkbox.isChecked()
        }
        
        # Disable controls
        self.analyze_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)
        
        # Start analysis thread
        self.log_message("Starting analysis...")
        self.analysis_thread = VideoAnalysisThread(
            self.video_path, self.output_path, self.model_path, config
        )
        self.analysis_thread.progress_update.connect(self.update_progress)
        self.analysis_thread.analysis_complete.connect(self.analysis_finished)
        self.analysis_thread.error_occurred.connect(self.analysis_error)
        self.analysis_thread.start()
    
    def stop_analysis(self):
        """Stop the current analysis"""
        if self.analysis_thread:
            self.log_message("Stopping analysis...")
            self.analysis_thread.stop()
            self.analysis_thread.wait()
            self.analysis_thread = None
            self.analyze_btn.setEnabled(True)
            self.stop_btn.setEnabled(False)
            self.progress_bar.setValue(0)
            self.status_label.setText("Analysis stopped")
    
    def update_progress(self, percentage, message):
        """Update progress bar and status"""
        self.progress_bar.setValue(percentage)
        self.status_label.setText(message)
        self.log_message(f"[{percentage}%] {message}")
    
    def analysis_finished(self, output_path):
        """Handle analysis completion"""
        self.analyze_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        self.log_message(f"Analysis complete! Output saved to: {output_path}")
        self.status_label.setText("Analysis complete!")
        
        # Load output video for playback
        if os.path.exists(output_path):
            self.video_player.load_video(output_path)
            self.log_message("Loaded output video for playback")
    
    def analysis_error(self, error_msg):
        """Handle analysis error"""
        self.analyze_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        self.log_message(f"ERROR: {error_msg}")
        self.status_label.setText("Analysis failed!")
    
    def log_message(self, message):
        """Add message to log"""
        self.log_text.append(message)
        # Auto-scroll to bottom
        scrollbar = self.log_text.verticalScrollBar()
        scrollbar.setValue(scrollbar.maximum())


def main():
    """Main application entry point"""
    app = QApplication(sys.argv)
    
    # Set application style
    app.setStyle('Fusion')
    
    window = FootballAnalysisGUI()
    window.show()
    
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
