#!/usr/bin/env python3
"""
===============================================================================
足球分析 - 主要流程管道
===============================================================================

這是足球影片分析系統的主要進入點。
它協調從影片輸入到帶有球員追蹤、隊伍分類和效能指標的註解輸出的整個分析管道。

執行流程：
1. 載入影片和 YOLO 模型
2. 偵測和追蹤球員、裁判和球（YOLO + ByteTrack）
3. 使用 K-means 聚類根據球衣顏色將球員分配到隊伍
4. 使用光流估計相機移動
5. 轉換透視以進行準確的距離測量
6. 計算球員速度和移動距離
7. 確定每個隊伍的控球權
8. 用邊界框、標籤和統計數據標註影片
9. 將結果匯出為影片（AVI）、CSV 和 JSON

匯入模組：
- utils: 影片輸入/輸出、邊界框工具、資料輸出
- trackers: 基於 YOLO 的物件偵測和追蹤
- team_assigner: 用於隊伍識別的 K-means 聚類
- player_ball_assigner: 控球偵測
- camera_movement_estimator: 相機運動的光流法
- view_transformer: 透視轉換
- speed_and_distance_estimator: 指標計算

使用方式：
由 Qt GUI 透過 QProcess 呼叫：
    python main.py --input <影片路徑> --model <模型路徑>

或直接從命令列執行：
    python main.py --input input_videos/match.mp4 --model models/best.pt
===============================================================================
"""

import os
import sys
import logging
import argparse
from pathlib import Path
from typing import Optional, List, Dict, Any

import cv2
import numpy as np

# 匯入影片分析管道的自訂模組
from utils import read_video, save_video, output_data
from trackers import Tracker
from team_assigner import TeamAssigner
from player_ball_assigner import PlayerBallAssigner
from camera_movement_estimator import CameraMovementEstimator
from view_transformer import ViewTransformer
from speed_and_distance_estimator import SpeedAndDistance_Estimator

# 設定管道監控的日誌記錄
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


################################################################################
# 影片分析管道類別
################################################################################

class VideoAnalysisPipeline:
    """
    具有完整錯誤處理的生產級影片分析管道。
    
    此類別封裝了整個足球影片分析工作流程，
    協調多個電腦視覺和機器學習元件
    以產生標註的影片和統計輸出。
    """
    
    def __init__(self, 
                 input_video_path: str,
                 model_path: str,
                 output_dir: str = 'output_videos',
                 use_stubs: bool = True):
        """
        初始化影片分析管道。
        
        參數：
            input_video_path: 輸入影片檔案的路徑
            model_path: YOLO 模型檔案的路徑
            output_dir: 輸出檔案的目錄
            use_stubs: 是否使用快取存根檔案以加快處理速度
        """
        self.input_video_path = input_video_path
        self.model_path = model_path
        self.output_dir = output_dir
        self.use_stubs = use_stubs
        
        # 提早驗證輸入（快速失敗）
        self._validate_inputs()
        
        # 建立輸出目錄
        self._setup_output_directory()
        
    def _validate_inputs(self) -> None:
        """驗證所有必需的輸入是否存在且可訪問。"""
        # 檢查輸入影片
        if not os.path.exists(self.input_video_path):
            raise FileNotFoundError(
                f"Input video not found: {self.input_video_path}"
            )
        
        # 檢查模型檔案
        if not os.path.exists(self.model_path):
            raise FileNotFoundError(
                f"Model file not found: {self.model_path}"
            )
        
        # 驗證輸入影片是否可讀取
        cap = cv2.VideoCapture(self.input_video_path)
        if not cap.isOpened():
            raise IOError(
                f"Cannot open video file: {self.input_video_path}"
            )
        cap.release()
        
        logger.info("Input validation passed")
    
    def _setup_output_directory(self) -> None:
        """建立輸出目錄（如果不存在）。"""
        try:
            os.makedirs(self.output_dir, exist_ok=True)
            logger.info(f"Output directory ready: {self.output_dir}")
        except Exception as e:
            raise IOError(
                f"Failed to create output directory {self.output_dir}: {e}"
            )
    
    def _read_video(self) -> List[np.ndarray]:
        """讀取影片幀並進行錯誤處理。"""
        try:
            logger.info(f"Reading video: {self.input_video_path}")
            frames = read_video(self.input_video_path)
            
            if not frames:
                raise ValueError("No frames read from video")
            
            logger.info(f"Successfully read {len(frames)} frames")
            return frames
        except Exception as e:
            raise RuntimeError(f"Failed to read video: {e}")
    
    # =========================================================================
    # 元件初始化
    # =========================================================================
    
    def _initialize_tracker(self) -> Tracker:
        """
        初始化基於 YOLO 的物件追蹤器。
        
        載入 YOLO 模型並設定 ByteTrack 進行物件追蹤。
        追蹤器將偵測球員、裁判、守門員和球。
        """
        try:
            logger.info("Initializing tracker")
            tracker = Tracker(self.model_path)
            return tracker
        except Exception as e:
            raise RuntimeError(f"Failed to initialize tracker: {e}")
    
    # =========================================================================
    # 偵測和追蹤
    # =========================================================================
    
    def _get_object_tracks(self, 
                          tracker: Tracker, 
                          frames: List[np.ndarray]) -> Dict[str, Any]:
        """
        偵測並追蹤所有物件（球員、裁判、球）跨幀。
        
        使用 YOLO 進行偵測，使用 ByteTrack 進行多物件追蹤。
        支援存根快取，以便在相同影片上更快地重複執行。
        
        返回：包含鍵值 'players'、'referees'、'ball' 的字典
             每個鍵包含逐幀的追蹤資料
        """
        """獲取支援存根的物件追蹤。"""
        try:
            stub_path = 'stubs/track_stubs.pkl' if self.use_stubs else None
            logger.info("Getting object tracks")
            
            tracks = tracker.get_object_tracks(
                frames,
                read_from_stub=self.use_stubs,
                stub_path=stub_path
            )
            
            if not tracks or 'players' not in tracks:
                raise ValueError("Invalid tracks data structure")
            
            logger.info("Object tracking complete")
            return tracks
        except Exception as e:
            raise RuntimeError(f"Failed to get object tracks: {e}")
    
    # =========================================================================
    # 相機移動補償
    # =========================================================================
    
    def _process_camera_movement(self,
                                 frames: List[np.ndarray],
                                 tracks: Dict[str, Any]) -> List[np.ndarray]:
        """
        使用光流估計和補償相機移動。
        
        追蹤影片中的特徵點以偵測相機的平移、傾斜和縮放。
        調整所有物件位置以考慮相機移動，使得
        能夠準確計算速度和距離。
        
        返回：每幀的相機移動作為 [dx, dy] 陣列
        """
        """處理相機移動估計。"""
        try:
            logger.info("Processing camera movement")
            
            estimator = CameraMovementEstimator(frames[0])
            stub_path = 'stubs/camera_movement_stub.pkl' if self.use_stubs else None
            
            camera_movement = estimator.get_camera_movement(
                frames,
                read_from_stub=self.use_stubs,
                stub_path=stub_path
            )
            
            estimator.add_adjust_positions_to_tracks(tracks, camera_movement)
            logger.info("Camera movement processing complete")
            
            return camera_movement
        except Exception as e:
            raise RuntimeError(f"Failed to process camera movement: {e}")
    
    # =========================================================================
    # 透視轉換
    # =========================================================================
    
    def _process_view_transformation(self, tracks: Dict[str, Any]) -> None:
        """
        應用透視轉換將像素座標轉換為真實世界座標。
        
        將 2D 影片座標映射到俯視場地視圖，實現準確的
        距離測量（以公尺為單位）。對於速度和距離計算至關重要。
        """
        """將視圖轉換應用於追蹤。"""
        try:
            logger.info("Applying view transformation")
            transformer = ViewTransformer()
            transformer.add_transformed_position_to_tracks(tracks)
            logger.info("View transformation complete")
        except Exception as e:
            raise RuntimeError(f"Failed to apply view transformation: {e}")
    
    # =========================================================================
    # 球追蹤增強
    # =========================================================================
    
    def _interpolate_ball_positions(self, 
                                    tracker: Tracker,
                                    tracks: Dict[str, Any]) -> None:
        """
        使用插值填補缺失的球偵測。
        
        由於遮擋或運動模糊，球的偵測可能是間歇性的。
        此方法使用插值來估計偵測失敗的幀中的球位置，
        創建流暢的球軌跡。
        """
        """插值球位置。"""
        try:
            logger.info("Interpolating ball positions")
            tracks["ball"] = tracker.interpolate_ball_positions(tracks["ball"])
            logger.info("Ball position interpolation complete")
        except Exception as e:
            raise RuntimeError(f"Failed to interpolate ball positions: {e}")
    
    # =========================================================================
    # 效能指標
    # =========================================================================
    
    def _estimate_speed_and_distance(self, tracks: Dict[str, Any]) -> None:
        """
        計算球員速度和移動距離。
        
        使用轉換後的位置和幀率計算：
        - 每幀每個球員的速度（公里/小時）
        - 每個球員移動的總距離（公尺）
        
        這些指標會新增到追蹤字典中並顯示在輸出中。
        """
        """計算速度和距離指標。"""
        try:
            logger.info("Calculating speed and distance")
            estimator = SpeedAndDistance_Estimator()
            estimator.add_speed_and_distance_to_tracks(tracks)
            logger.info("Speed and distance calculation complete")
        except Exception as e:
            raise RuntimeError(f"Failed to calculate speed and distance: {e}")
    
    # =========================================================================
    # 隊伍分配
    # =========================================================================
    
    def _assign_teams(self, 
                     frames: List[np.ndarray],
                     tracks: Dict[str, Any]) -> TeamAssigner:
        """
        根據球衣顏色將球員分類到隊伍。
        
        對球員球衣顏色使用 K-means 聚類來識別兩個隊伍。
        分析球員邊界框的上半部分（球衣可見的地方）
        以提取主要顏色並分配隊伍成員資格。
        
        返回：具有隊伍顏色分配的 TeamAssigner 實例
        """
        """將球員分配到隊伍。"""
        try:
            logger.info("Assigning player teams")
            
            if not tracks['players'] or not tracks['players'][0]:
                raise ValueError("No players detected in first frame")
            
            assigner = TeamAssigner()
            assigner.assign_team_color(frames[0], tracks['players'][0])
            
            for frame_num, player_track in enumerate(tracks['players']):
                for player_id, track in player_track.items():
                    team = assigner.get_player_team(
                        frames[frame_num],
                        track['bbox'],
                        player_id
                    )
                    tracks['players'][frame_num][player_id]['team'] = team
                    tracks['players'][frame_num][player_id]['team_color'] = \
                        assigner.team_colors[team]
            
            logger.info("Team assignment complete")
            return assigner
        except Exception as e:
            raise RuntimeError(f"Failed to assign teams: {e}")
    
    def _assign_ball_possession(self, tracks: Dict[str, Any]) -> np.ndarray:
        """確定每幀的控球權。"""
        try:
            logger.info("Assigning ball possession")
            
            assigner = PlayerBallAssigner()
            team_ball_control = []
            
            for frame_num, player_track in enumerate(tracks['players']):
                if 1 not in tracks['ball'][frame_num]:
                    # 未偵測到球，使用先前的控球權
                    if team_ball_control:
                        team_ball_control.append(team_ball_control[-1])
                    else:
                        team_ball_control.append(0)
                    continue
                
                ball_bbox = tracks['ball'][frame_num][1]['bbox']
                assigned_player = assigner.assign_ball_to_player(
                    player_track, 
                    ball_bbox
                )
                
                if assigned_player != -1:
                    tracks['players'][frame_num][assigned_player]['has_ball'] = True
                    team_ball_control.append(
                        tracks['players'][frame_num][assigned_player]['team']
                    )
                else:
                    # 無分配，使用先前的控球權
                    if team_ball_control:
                        team_ball_control.append(team_ball_control[-1])
                    else:
                        team_ball_control.append(0)
            
            logger.info("Ball possession assignment complete")
            return np.array(team_ball_control)
        except Exception as e:
            raise RuntimeError(f"Failed to assign ball possession: {e}")
    
    def _draw_annotations(self,
                         tracker: Tracker,
                         frames: List[np.ndarray],
                         tracks: Dict[str, Any],
                         team_ball_control: np.ndarray,
                         camera_movement: List[np.ndarray]) -> List[np.ndarray]:
        """在影片幀上繪製所有註解。"""
        try:
            logger.info("Drawing annotations")
            
            # 繪製物件追蹤
            output_frames = tracker.draw_annotations(
                frames, 
                tracks, 
                team_ball_control
            )
            
            # 繪製相機移動
            estimator = CameraMovementEstimator(frames[0])
            output_frames = estimator.draw_camera_movement(
                output_frames,
                camera_movement
            )
            
            # 繪製速度和距離
            speed_estimator = SpeedAndDistance_Estimator()
            speed_estimator.draw_speed_and_distance(output_frames, tracks)
            
            logger.info("Annotation drawing complete")
            return output_frames
        except Exception as e:
            raise RuntimeError(f"Failed to draw annotations: {e}")
    
    def _save_output_video(self, frames: List[np.ndarray]) -> str:
        """儲存輸出影片並進行健全的錯誤處理。"""
        try:
            output_path = os.path.join(self.output_dir, 'output_video.avi')
            logger.info(f"Saving output video to: {output_path}")
            
            if not frames:
                raise ValueError("No frames to save")
            
            save_video(frames, output_path)
            
            # 驗證輸出檔案是否已建立
            if not os.path.exists(output_path):
                raise IOError("Output video file was not created")
            
            file_size = os.path.getsize(output_path)
            logger.info(
                f"Output video saved successfully ({file_size / 1024 / 1024:.2f} MB)"
            )
            
            return output_path
        except Exception as e:
            raise RuntimeError(f"Failed to save output video: {e}")
    
    def _save_output_data(self, tracks: Dict[str, Any], team_ball_control: np.ndarray) -> str:
        """儲存輸出資料並進行錯誤處理。"""
        try:
            output_path = os.path.join(self.output_dir, 'data_output.json')
            logger.info(f"Saving output data to: {output_path}")
            
            output_data(tracks, output_path, team_ball_control)
            
            # 驗證輸出檔案是否已建立
            if not os.path.exists(output_path):
                raise IOError("Output data file was not created")
            
            csv_path = output_path.replace('.json', '.csv')
            if not os.path.exists(csv_path):
                raise IOError("Output CSV file was not created")
            
            logger.info("Output data saved successfully")
            return output_path
        except Exception as e:
            raise RuntimeError(f"Failed to save output data: {e}")
    
    def run(self) -> None:
        """執行完整的影片分析管道。"""
        try:
            logger.info("="*60)
            logger.info("Starting Football Analysis Pipeline")
            logger.info("="*60)
            
            # 讀取影片
            video_frames = self._read_video()
            
            # 初始化追蹤器
            tracker = self._initialize_tracker()
            
            # 獲取物件追蹤
            tracks = self._get_object_tracks(tracker, video_frames)
            
            # 將位置新增到追蹤
            tracker.add_position_to_tracks(tracks)
            
            # 處理相機移動
            camera_movement = self._process_camera_movement(video_frames, tracks)
            
            # 應用視圖轉換
            self._process_view_transformation(tracks)
            
            # 插值球位置
            self._interpolate_ball_positions(tracker, tracks)
            
            # 計算速度和距離
            self._estimate_speed_and_distance(tracks)
            
            # 分配隊伍
            self._assign_teams(video_frames, tracks)
            
            # 分配控球權
            team_ball_control = self._assign_ball_possession(tracks)
            
            # 繪製註解
            output_frames = self._draw_annotations(
                tracker,
                video_frames,
                tracks,
                team_ball_control,
                camera_movement
            )
            
            # 儲存輸出
            video_path = self._save_output_video(output_frames)
            data_path = self._save_output_data(tracks, team_ball_control)
            
            logger.info("="*60)
            logger.info("Pipeline completed successfully!")
            logger.info(f"Video output: {video_path}")
            logger.info(f"Data output: {data_path}")
            logger.info("="*60)
            
        except Exception as e:
            logger.error("="*60)
            logger.error(f"Pipeline failed: {e}")
            logger.error("="*60)
            raise


def main():
    """具有命令列參數支援的主要進入點。"""
    try:
        # 解析命令列參數
        parser = argparse.ArgumentParser(
            description='Football Analysis - Video Analysis Pipeline'
        )
        parser.add_argument(
            '--input',
            type=str,
            default='input_videos/08fd33_4.mp4',
            help='Path to input video file'
        )
        parser.add_argument(
            '--model',
            type=str,
            default='models/best.pt',
            help='Path to YOLO model file'
        )
        parser.add_argument(
            '--output',
            type=str,
            default='output_videos',
            help='Output directory for results'
        )
        parser.add_argument(
            '--no-cache',
            action='store_true',
            help='Disable cached stub files'
        )
        
        args = parser.parse_args()
        
        # 取得此腳本所在的目錄
        script_dir = os.path.dirname(os.path.abspath(__file__))
        
        # 如果路徑不是絕對路徑，則相對於腳本目錄解析路徑
        def resolve_path(path: str) -> str:
            """如果不是絕對路徑，則相對於腳本目錄解析路徑。"""
            if os.path.isabs(path):
                return path
            return os.path.join(script_dir, path)
        
        input_video = resolve_path(args.input)
        model_file = resolve_path(args.model)
        output_directory = resolve_path(args.output)
        use_cached_stubs = not args.no_cache
        
        logger.info(f"Input video: {input_video}")
        logger.info(f"Model file: {model_file}")
        logger.info(f"Output directory: {output_directory}")
        
        # 建立並執行管道
        pipeline = VideoAnalysisPipeline(
            input_video_path=input_video,
            model_path=model_file,
            output_dir=output_directory,
            use_stubs=use_cached_stubs
        )
        
        pipeline.run()
        
        return 0
        
    except KeyboardInterrupt:
        logger.warning("Process interrupted by user")
        return 130
    except Exception as e:
        logger.exception(f"Fatal error: {e}")
        return 1


if __name__ == '__main__':
    sys.exit(main())