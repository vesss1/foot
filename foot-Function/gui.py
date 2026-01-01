#!/usr/bin/env python3
"""
Football Analysis Qt GUI Application
Provides a graphical interface for video analysis with all input/output features.
"""

import sys
import os
import logging
from pathlib import Path
from typing import Optional
import json
import csv

from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QLabel, QLineEdit, QFileDialog, QTextEdit,
    QProgressBar, QGroupBox, QTableWidget, QTableWidgetItem,
    QTabWidget, QMessageBox, QCheckBox, QSplitter
)
from PyQt5.QtCore import QThread, pyqtSignal, Qt
from PyQt5.QtGui import QFont, QTextCursor


class AnalysisThread(QThread):
    """Worker thread for running video analysis without blocking the GUI."""
    
    progress_update = pyqtSignal(str)  # Signal for status updates
    error_occurred = pyqtSignal(str)   # Signal for errors
    analysis_complete = pyqtSignal(str, str)  # Signal for completion (video_path, data_path)
    
    def __init__(self, input_video, model_path, output_dir, use_stubs):
        super().__init__()
        self.input_video = input_video
        self.model_path = model_path
        self.output_dir = output_dir
        self.use_stubs = use_stubs
        self._is_running = True
        
    def run(self):
        """Execute the video analysis pipeline."""
        try:
            # Import here to avoid loading heavy dependencies at startup
            from main import VideoAnalysisPipeline
            
            self.progress_update.emit("Starting analysis pipeline...")
            
            # Create pipeline
            pipeline = VideoAnalysisPipeline(
                input_video_path=self.input_video,
                model_path=self.model_path,
                output_dir=self.output_dir,
                use_stubs=self.use_stubs
            )
            
            # Run analysis
            pipeline.run()
            
            # Construct output paths
            video_output = os.path.join(self.output_dir, 'output_video.avi')
            data_output = os.path.join(self.output_dir, 'data_output.json')
            
            self.progress_update.emit("Analysis completed successfully!")
            self.analysis_complete.emit(video_output, data_output)
            
        except Exception as e:
            error_msg = f"Analysis failed: {str(e)}"
            self.progress_update.emit(error_msg)
            self.error_occurred.emit(error_msg)
    
    def stop(self):
        """Stop the analysis (note: actual stop implementation would need pipeline support)."""
        self._is_running = False
        self.terminate()


class FootballAnalysisGUI(QMainWindow):
    """Main GUI window for football video analysis."""
    
    def __init__(self):
        super().__init__()
        self.analysis_thread = None
        self.setup_logging()
        self.init_ui()
        
    def setup_logging(self):
        """Configure logging to capture messages in GUI."""
        # Create a custom handler that emits to GUI
        self.log_handler = logging.StreamHandler()
        self.log_handler.setLevel(logging.INFO)
        formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
        self.log_handler.setFormatter(formatter)
        
    def init_ui(self):
        """Initialize the user interface."""
        self.setWindowTitle('Football Analysis System - Qt GUI')
        self.setGeometry(100, 100, 1200, 800)
        
        # Create central widget and main layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Create tab widget for different sections
        tab_widget = QTabWidget()
        main_layout.addWidget(tab_widget)
        
        # Tab 1: Input/Configuration
        config_tab = self.create_config_tab()
        tab_widget.addTab(config_tab, "Configuration")
        
        # Tab 2: Analysis Progress
        progress_tab = self.create_progress_tab()
        tab_widget.addTab(progress_tab, "Analysis Progress")
        
        # Tab 3: Results
        results_tab = self.create_results_tab()
        tab_widget.addTab(results_tab, "Results")
        
        # Status bar
        self.statusBar().showMessage('Ready')
        
    def create_config_tab(self):
        """Create the configuration tab with input/output settings."""
        widget = QWidget()
        layout = QVBoxLayout(widget)
        
        # Input Video Section
        input_group = QGroupBox("Input Video")
        input_layout = QVBoxLayout()
        
        input_file_layout = QHBoxLayout()
        self.input_video_edit = QLineEdit()
        self.input_video_edit.setPlaceholderText("Select input video file...")
        input_file_layout.addWidget(QLabel("Video File:"))
        input_file_layout.addWidget(self.input_video_edit)
        
        input_browse_btn = QPushButton("Browse...")
        input_browse_btn.clicked.connect(self.browse_input_video)
        input_file_layout.addWidget(input_browse_btn)
        
        input_layout.addLayout(input_file_layout)
        input_group.setLayout(input_layout)
        layout.addWidget(input_group)
        
        # Model Section
        model_group = QGroupBox("YOLO Model")
        model_layout = QVBoxLayout()
        
        model_file_layout = QHBoxLayout()
        self.model_path_edit = QLineEdit()
        self.model_path_edit.setPlaceholderText("Select YOLO model file (.pt)...")
        model_file_layout.addWidget(QLabel("Model File:"))
        model_file_layout.addWidget(self.model_path_edit)
        
        model_browse_btn = QPushButton("Browse...")
        model_browse_btn.clicked.connect(self.browse_model)
        model_file_layout.addWidget(model_browse_btn)
        
        model_layout.addLayout(model_file_layout)
        model_group.setLayout(model_layout)
        layout.addWidget(model_group)
        
        # Output Directory Section
        output_group = QGroupBox("Output Settings")
        output_layout = QVBoxLayout()
        
        output_dir_layout = QHBoxLayout()
        self.output_dir_edit = QLineEdit()
        self.output_dir_edit.setText("output_videos")
        self.output_dir_edit.setPlaceholderText("Select output directory...")
        output_dir_layout.addWidget(QLabel("Output Directory:"))
        output_dir_layout.addWidget(self.output_dir_edit)
        
        output_browse_btn = QPushButton("Browse...")
        output_browse_btn.clicked.connect(self.browse_output_dir)
        output_dir_layout.addWidget(output_browse_btn)
        
        output_layout.addLayout(output_dir_layout)
        
        # Use cache checkbox
        self.use_stubs_checkbox = QCheckBox("Use cached stub files (faster processing)")
        self.use_stubs_checkbox.setChecked(True)
        output_layout.addWidget(self.use_stubs_checkbox)
        
        output_group.setLayout(output_layout)
        layout.addWidget(output_group)
        
        # Control Buttons
        button_layout = QHBoxLayout()
        
        self.start_btn = QPushButton("Start Analysis")
        self.start_btn.setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 10px;")
        self.start_btn.clicked.connect(self.start_analysis)
        button_layout.addWidget(self.start_btn)
        
        self.stop_btn = QPushButton("Stop Analysis")
        self.stop_btn.setStyleSheet("background-color: #f44336; color: white; font-weight: bold; padding: 10px;")
        self.stop_btn.setEnabled(False)
        self.stop_btn.clicked.connect(self.stop_analysis)
        button_layout.addWidget(self.stop_btn)
        
        layout.addLayout(button_layout)
        
        # Add stretch to push everything to top
        layout.addStretch()
        
        return widget
    
    def create_progress_tab(self):
        """Create the progress monitoring tab."""
        widget = QWidget()
        layout = QVBoxLayout(widget)
        
        # Progress bar
        progress_group = QGroupBox("Analysis Progress")
        progress_layout = QVBoxLayout()
        
        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 0)  # Indeterminate progress
        self.progress_bar.setVisible(False)
        progress_layout.addWidget(self.progress_bar)
        
        progress_group.setLayout(progress_layout)
        layout.addWidget(progress_group)
        
        # Log viewer
        log_group = QGroupBox("Analysis Log")
        log_layout = QVBoxLayout()
        
        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setFont(QFont("Courier", 9))
        log_layout.addWidget(self.log_text)
        
        # Clear log button
        clear_btn = QPushButton("Clear Log")
        clear_btn.clicked.connect(self.log_text.clear)
        log_layout.addWidget(clear_btn)
        
        log_group.setLayout(log_layout)
        layout.addWidget(log_group)
        
        return widget
    
    def create_results_tab(self):
        """Create the results display tab."""
        widget = QWidget()
        layout = QVBoxLayout(widget)
        
        # Output files section
        files_group = QGroupBox("Output Files")
        files_layout = QVBoxLayout()
        
        # Video output
        video_layout = QHBoxLayout()
        video_layout.addWidget(QLabel("Video Output:"))
        self.video_output_label = QLabel("Not generated yet")
        self.video_output_label.setWordWrap(True)
        video_layout.addWidget(self.video_output_label, 1)
        
        self.open_video_btn = QPushButton("Open Video")
        self.open_video_btn.setEnabled(False)
        self.open_video_btn.clicked.connect(self.open_output_video)
        video_layout.addWidget(self.open_video_btn)
        files_layout.addLayout(video_layout)
        
        # Data output
        data_layout = QHBoxLayout()
        data_layout.addWidget(QLabel("Data Output:"))
        self.data_output_label = QLabel("Not generated yet")
        self.data_output_label.setWordWrap(True)
        data_layout.addWidget(self.data_output_label, 1)
        
        self.open_data_btn = QPushButton("View Data")
        self.open_data_btn.setEnabled(False)
        self.open_data_btn.clicked.connect(self.view_output_data)
        data_layout.addWidget(self.open_data_btn)
        files_layout.addLayout(data_layout)
        
        files_group.setLayout(files_layout)
        layout.addWidget(files_group)
        
        # Statistics display
        stats_group = QGroupBox("Analysis Statistics")
        stats_layout = QVBoxLayout()
        
        # Team possession
        self.possession_label = QLabel("Team Possession: Not available")
        self.possession_label.setFont(QFont("Arial", 11, QFont.Bold))
        stats_layout.addWidget(self.possession_label)
        
        # Player statistics table
        self.stats_table = QTableWidget()
        self.stats_table.setColumnCount(3)
        self.stats_table.setHorizontalHeaderLabels(["Team", "Player ID", "Distance (m)"])
        stats_layout.addWidget(self.stats_table)
        
        stats_group.setLayout(stats_layout)
        layout.addWidget(stats_group)
        
        return widget
    
    def browse_input_video(self):
        """Open file dialog to select input video."""
        file_path, _ = QFileDialog.getOpenFileName(
            self,
            "Select Input Video",
            "",
            "Video Files (*.mp4 *.avi *.mov *.mkv);;All Files (*.*)"
        )
        if file_path:
            self.input_video_edit.setText(file_path)
    
    def browse_model(self):
        """Open file dialog to select YOLO model."""
        file_path, _ = QFileDialog.getOpenFileName(
            self,
            "Select YOLO Model",
            "",
            "PyTorch Model (*.pt);;All Files (*.*)"
        )
        if file_path:
            self.model_path_edit.setText(file_path)
    
    def browse_output_dir(self):
        """Open directory dialog to select output directory."""
        dir_path = QFileDialog.getExistingDirectory(
            self,
            "Select Output Directory"
        )
        if dir_path:
            self.output_dir_edit.setText(dir_path)
    
    def start_analysis(self):
        """Start the video analysis process."""
        # Validate inputs
        input_video = self.input_video_edit.text().strip()
        model_path = self.model_path_edit.text().strip()
        output_dir = self.output_dir_edit.text().strip()
        
        if not input_video:
            QMessageBox.warning(self, "Input Required", "Please select an input video file.")
            return
        
        if not os.path.exists(input_video):
            QMessageBox.warning(self, "File Not Found", f"Input video file not found: {input_video}")
            return
        
        if not model_path:
            QMessageBox.warning(self, "Input Required", "Please select a YOLO model file.")
            return
        
        if not os.path.exists(model_path):
            QMessageBox.warning(self, "File Not Found", f"Model file not found: {model_path}")
            return
        
        if not output_dir:
            QMessageBox.warning(self, "Input Required", "Please specify an output directory.")
            return
        
        # Disable start button and enable stop button
        self.start_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)
        
        # Show progress bar
        self.progress_bar.setVisible(True)
        
        # Clear previous log
        self.log_text.clear()
        self.add_log("Starting football analysis...", "INFO")
        
        # Get settings
        use_stubs = self.use_stubs_checkbox.isChecked()
        
        # Create and start analysis thread
        self.analysis_thread = AnalysisThread(
            input_video,
            model_path,
            output_dir,
            use_stubs
        )
        
        # Connect signals
        self.analysis_thread.progress_update.connect(self.update_progress)
        self.analysis_thread.error_occurred.connect(self.handle_error)
        self.analysis_thread.analysis_complete.connect(self.handle_completion)
        
        # Start thread
        self.analysis_thread.start()
        
        self.statusBar().showMessage('Analysis in progress...')
    
    def stop_analysis(self):
        """Stop the running analysis."""
        if self.analysis_thread and self.analysis_thread.isRunning():
            self.add_log("Stopping analysis...", "WARNING")
            self.analysis_thread.stop()
            self.analysis_thread.wait()
            self.reset_buttons()
            self.statusBar().showMessage('Analysis stopped by user')
    
    def update_progress(self, message):
        """Update progress display with a message."""
        self.add_log(message, "INFO")
    
    def handle_error(self, error_message):
        """Handle errors from analysis thread."""
        self.add_log(error_message, "ERROR")
        QMessageBox.critical(self, "Analysis Error", error_message)
        self.reset_buttons()
        self.statusBar().showMessage('Analysis failed')
    
    def handle_completion(self, video_path, data_path):
        """Handle successful completion of analysis."""
        self.add_log("Analysis completed successfully!", "SUCCESS")
        self.reset_buttons()
        self.progress_bar.setVisible(False)
        
        # Update results tab
        self.video_output_label.setText(video_path)
        self.data_output_label.setText(data_path)
        self.open_video_btn.setEnabled(True)
        self.open_data_btn.setEnabled(True)
        
        # Load and display results
        self.load_results(data_path)
        
        self.statusBar().showMessage('Analysis completed successfully!')
        
        # Show completion message
        QMessageBox.information(
            self,
            "Analysis Complete",
            f"Video analysis completed successfully!\n\nOutput video: {video_path}\nData output: {data_path}"
        )
    
    def reset_buttons(self):
        """Reset button states after analysis."""
        self.start_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        self.progress_bar.setVisible(False)
    
    def add_log(self, message, level="INFO"):
        """Add a message to the log viewer."""
        # Color code by level
        color_map = {
            "INFO": "black",
            "WARNING": "orange",
            "ERROR": "red",
            "SUCCESS": "green"
        }
        
        color = color_map.get(level, "black")
        formatted_message = f'<span style="color: {color};">[{level}] {message}</span>'
        
        self.log_text.append(formatted_message)
        # Auto-scroll to bottom
        cursor = self.log_text.textCursor()
        cursor.movePosition(QTextCursor.End)
        self.log_text.setTextCursor(cursor)
    
    def open_output_video(self):
        """Open the output video with default system player."""
        video_path = self.video_output_label.text()
        if os.path.exists(video_path):
            import subprocess
            import platform
            
            try:
                if platform.system() == 'Darwin':  # macOS
                    subprocess.call(['open', video_path])
                elif platform.system() == 'Windows':
                    os.startfile(video_path)
                else:  # Linux
                    subprocess.call(['xdg-open', video_path])
            except Exception as e:
                QMessageBox.warning(self, "Error", f"Could not open video: {str(e)}")
        else:
            QMessageBox.warning(self, "File Not Found", "Output video file not found.")
    
    def view_output_data(self):
        """Load and display output data."""
        data_path = self.data_output_label.text()
        if os.path.exists(data_path):
            self.load_results(data_path)
        else:
            QMessageBox.warning(self, "File Not Found", "Output data file not found.")
    
    def load_results(self, data_path):
        """Load and display analysis results."""
        try:
            # Load JSON data
            with open(data_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
            
            # Update team possession
            summary = data.get('summary', {})
            team1_poss = summary.get('team_1_possession_percent', 0)
            team2_poss = summary.get('team_2_possession_percent', 0)
            
            possession_text = f"Team Possession - Team 1: {team1_poss}% | Team 2: {team2_poss}%"
            self.possession_label.setText(possession_text)
            
            # Update statistics table
            self.stats_table.setRowCount(0)
            
            row = 0
            for team_key in sorted(data.keys()):
                if team_key == 'summary':
                    continue
                
                players_data = data[team_key]
                for player_id, player_stats in players_data.items():
                    self.stats_table.insertRow(row)
                    
                    self.stats_table.setItem(row, 0, QTableWidgetItem(team_key))
                    self.stats_table.setItem(row, 1, QTableWidgetItem(player_id))
                    
                    distance = player_stats.get('distance_m', 0)
                    distance_text = "Not Detected" if distance == 0 else f"{distance:.2f}"
                    self.stats_table.setItem(row, 2, QTableWidgetItem(distance_text))
                    
                    row += 1
            
            self.stats_table.resizeColumnsToContents()
            
        except Exception as e:
            QMessageBox.warning(self, "Error", f"Could not load results: {str(e)}")


def main():
    """Main entry point for the Qt GUI application."""
    app = QApplication(sys.argv)
    
    # Set application style
    app.setStyle('Fusion')
    
    # Create and show main window
    window = FootballAnalysisGUI()
    window.show()
    
    # Run application
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
