#!/usr/bin/env python3
"""
Football Analysis with Qt UI Launcher
Runs the analysis pipeline and launches the Qt UI to view results
"""

import sys
import os
import argparse
import logging
from pathlib import Path

# Setup logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


def run_analysis():
    """Run the football analysis pipeline"""
    try:
        from main import VideoAnalysisPipeline
        
        logger.info("Starting video analysis pipeline...")
        
        # Configuration
        input_video = 'input_videos/08fd33_4.mp4'
        model_file = 'models/best.pt'
        output_directory = 'output_videos'
        use_cached_stubs = True
        
        # Create and run pipeline
        pipeline = VideoAnalysisPipeline(
            input_video_path=input_video,
            model_path=model_file,
            output_dir=output_directory,
            use_stubs=use_cached_stubs
        )
        
        pipeline.run()
        
        logger.info("Analysis pipeline completed successfully!")
        return True
        
    except FileNotFoundError as e:
        logger.error(f"File not found: {e}")
        logger.error("Please ensure input video and model files are in the correct locations.")
        return False
    except Exception as e:
        logger.error(f"Analysis failed: {e}")
        return False


def launch_ui():
    """Launch the Qt UI application"""
    try:
        from qt_ui_app import main as ui_main
        
        logger.info("Launching Qt UI application...")
        ui_main()
        
    except ImportError as e:
        logger.error(f"Failed to import Qt UI: {e}")
        logger.error("Please ensure PyQt5 is installed: pip install PyQt5")
        sys.exit(1)
    except Exception as e:
        logger.error(f"UI launch failed: {e}")
        sys.exit(1)


def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description='Football Analysis with Qt UI',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run analysis then launch UI
  python run_analysis_with_ui.py --run-analysis
  
  # Just launch UI (if analysis already done)
  python run_analysis_with_ui.py
  
  # Only run analysis, don't launch UI
  python run_analysis_with_ui.py --run-analysis --no-ui
        """
    )
    
    parser.add_argument(
        '--run-analysis',
        action='store_true',
        help='Run the analysis pipeline before launching UI'
    )
    
    parser.add_argument(
        '--no-ui',
        action='store_true',
        help='Skip launching the UI (only with --run-analysis)'
    )
    
    args = parser.parse_args()
    
    # Run analysis if requested
    if args.run_analysis:
        success = run_analysis()
        if not success:
            logger.error("Analysis failed, exiting...")
            return 1
    
    # Launch UI unless --no-ui specified
    if not args.no_ui:
        launch_ui()
    
    return 0


if __name__ == '__main__':
    sys.exit(main())
