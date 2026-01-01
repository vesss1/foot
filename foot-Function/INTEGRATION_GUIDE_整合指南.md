# Qt UI 與分析管線整合說明
# Qt UI Integration with Analysis Pipeline

## 整合架構 / Integration Architecture

```
Football Analysis Pipeline (main.py)
    ↓
    ↓ 產生輸出 / Generates output
    ↓
output_videos/
    ├── output_video.avi     ← 影片輸出 / Video output
    ├── data_output.json     ← 資料輸出 / Data output
    └── data_output.csv      ← CSV 格式 / CSV format
    ↓
    ↓ 讀取 / Read by
    ↓
Qt UI Application (qt_ui_app.py)
    ├── Video Player         ← 播放 output_video.avi
    └── Statistics Display   ← 顯示 data_output.json
```

## 資料流程 / Data Flow

### 1. 分析管線輸出 / Analysis Pipeline Output

分析管線 (main.py) 產生以下資料：

Analysis pipeline (main.py) generates:

```python
# In main.py (lines 319-338)
def _save_output_data(self, tracks, team_ball_control):
    """Save output data"""
    output_path = os.path.join(self.output_dir, 'data_output.json')
    output_data(tracks, output_path, team_ball_control)
```

產生的 JSON 結構 / Generated JSON structure:
```json
{
  "summary": {
    "team_1_possession_percent": 55.23,
    "team_2_possession_percent": 44.77
  },
  "team_1": {
    "1": {"distance_km": 0.523, "distance_m": 523.45},
    "3": {"distance_km": 0.678, "distance_m": 678.12}
  },
  "team_2": {
    "2": {"distance_km": 0.445, "distance_m": 445.78},
    "4": {"distance_km": 0.567, "distance_m": 567.23}
  }
}
```

### 2. Qt UI 讀取 / Qt UI Reading

Qt UI (qt_ui_app.py) 讀取並顯示資料：

Qt UI (qt_ui_app.py) reads and displays data:

```python
# In qt_ui_app.py (lines 419-443)
def load_statistics(self, file_path=None):
    """Load statistics from JSON file"""
    if file_path is None:
        file_path = os.path.join(self.output_dir, "data_output.json")
    
    with open(file_path, 'r', encoding='utf-8') as f:
        self.stats_data = json.load(f)
    
    self.update_statistics_display()
```

### 3. 顯示映射 / Display Mapping

資料到 UI 的映射關係：

Data to UI mapping:

| 資料來源 / Data Source | UI 元素 / UI Element | 顯示內容 / Display |
|------------------------|---------------------|-------------------|
| `summary.team_1_possession_percent` | `team1_possession_label` | "55.2%" (藍色/blue) |
| `summary.team_2_possession_percent` | `team2_possession_label` | "44.8%" (紅色/red) |
| Sum of all `distance_km` | `total_distance_label` | "5.87 km" (綠色/green) |
| Average of `distance_km` | `avg_distance_label` | "0.53 km" |
| Video duration | `video_duration_label` | "10.0 s" |
| Calculated speed | `avg_speed_label` | "2.11 km/h" |

## 使用場景 / Usage Scenarios

### 場景 1：完整流程 / Scenario 1: Complete Flow

```bash
# 1. 執行分析管線
# 1. Run analysis pipeline
cd foot-Function
python main.py

# 這會產生：
# This generates:
# - output_videos/output_video.avi
# - output_videos/data_output.json
# - output_videos/data_output.csv

# 2. 啟動 Qt UI 查看結果
# 2. Launch Qt UI to view results
python qt_ui_app.py
# UI 會自動載入 output_videos/ 中的檔案
# UI automatically loads files from output_videos/
```

### 場景 2：使用啟動器 / Scenario 2: Using Launcher

```bash
# 一鍵執行分析並啟動 UI
# Run analysis and launch UI in one command
python run_analysis_with_ui.py --run-analysis
```

### 場景 3：測試模式 / Scenario 3: Test Mode

```bash
# 產生測試資料並測試 UI
# Generate test data and test UI
python generate_test_data.py  # 產生測試檔案
python qt_ui_app.py            # 查看測試資料
```

## 整合點詳細說明 / Integration Points Detail

### A. 影片整合 / Video Integration

**分析管線側 / Analysis Pipeline Side:**
```python
# main.py (lines 295-316)
def _save_output_video(self, frames):
    output_path = os.path.join(self.output_dir, 'output_video.avi')
    save_video(frames, output_path)
```

**Qt UI 側 / Qt UI Side:**
```python
# qt_ui_app.py (lines 236-289)
def load_video(self, file_path):
    self.video_capture = cv2.VideoCapture(file_path)
    # Load frames and setup playback
```

### B. 統計資料整合 / Statistics Integration

**分析管線側 / Analysis Pipeline Side:**
```python
# utils/data_output.py (lines 7-119)
def output_data(tracks, output_path, team_ball_control):
    # Calculate possession percentages
    # Extract player distances
    # Save to JSON and CSV
```

**Qt UI 側 / Qt UI Side:**
```python
# qt_ui_app.py (lines 445-490)
def update_statistics_display(self):
    # Read possession from summary
    # Calculate total distances
    # Update all labels
```

## 資料格式說明 / Data Format Details

### 控球率計算 / Possession Calculation

```python
# In utils/data_output.py (lines 28-45)
unique_teams, counts = np.unique(team_ball_control, return_counts=True)
total_possession = sum(possession_counts.values())
percent = (count / total_possession) * 100
```

### 距離計算 / Distance Calculation

```python
# In qt_ui_app.py (lines 459-474)
for key in self.stats_data:
    if key.startswith('team_'):
        for player_id, player_data in team_data.items():
            distance_km = player_data.get('distance_km', 0)
            total_distance += distance_km
            player_count += 1
```

### 速度計算 / Speed Calculation

```python
# In qt_ui_app.py (lines 479-490)
duration_hours = duration_sec / 3600
avg_speed = avg_distance / duration_hours  # km/h
```

## 擴展性 / Extensibility

要新增更多統計資料顯示，只需：

To add more statistics display:

1. 在 `utils/data_output.py` 中產生資料
   Generate data in `utils/data_output.py`

2. 在 `qt_ui_app.py` 的 `create_stats_panel()` 中新增 UI 元素
   Add UI elements in `create_stats_panel()` in `qt_ui_app.py`

3. 在 `update_statistics_display()` 中更新顯示
   Update display in `update_statistics_display()`

範例：新增射門統計 / Example: Add shot statistics:

```python
# Step 1: In data_output.py
output_dict['summary']['total_shots'] = calculate_shots(tracks)

# Step 2: In qt_ui_app.py create_stats_panel()
self.shots_label = QLabel("--- shots")
stats_layout.addWidget(self.shots_label)

# Step 3: In update_statistics_display()
shots = summary.get('total_shots', 0)
self.shots_label.setText(f"{shots} shots")
```

## 錯誤處理 / Error Handling

UI 處理以下錯誤情況：

UI handles these error cases:

1. **檔案不存在** / File not found
   - 顯示友善訊息 / Show friendly message
   - 提供載入檔案按鈕 / Provide load file button

2. **影片格式錯誤** / Invalid video format
   - OpenCV 讀取失敗時顯示錯誤 / Show error on read failure

3. **JSON 格式錯誤** / Invalid JSON format
   - 特定異常處理 / Specific exception handling
   - 詳細錯誤訊息 / Detailed error message

4. **記憶體不足** / Out of memory
   - 限制最大幀數 / Limit max frames
   - 顯示截斷警告 / Show truncation warning

## 效能優化 / Performance Optimization

目前實作已包含：

Current implementation includes:

- ✓ 預載所有幀到記憶體（平滑播放）
  Pre-load all frames to memory (smooth playback)
  
- ✓ 最大幀數限制（防止記憶體溢出）
  Max frame limit (prevent memory overflow)
  
- ✓ FPS 驗證（防止除以零）
  FPS validation (prevent division by zero)

未來可優化：

Future optimizations:

- 實作幀緩衝區（懶載入）
  Implement frame buffer (lazy loading)
  
- 硬體加速解碼
  Hardware-accelerated decoding
  
- 多執行緒載入
  Multi-threaded loading

## 總結 / Summary

Qt UI 與分析管線的整合非常簡潔：

Integration between Qt UI and analysis pipeline is clean:

1. **鬆耦合設計** / Loose coupling: 透過檔案系統通訊
2. **標準格式** / Standard formats: JSON, CSV, AVI
3. **自動載入** / Auto-loading: 便利的使用者體驗
4. **錯誤處理** / Error handling: 健全的錯誤處理
5. **易於擴展** / Easy to extend: 模組化設計

使用者只需關注兩個命令：

Users only need two commands:

```bash
python main.py              # 執行分析 / Run analysis
python qt_ui_app.py         # 查看結果 / View results
```

或使用整合啟動器 / Or use integrated launcher:

```bash
python run_analysis_with_ui.py --run-analysis
```

---

**整合狀態 / Integration Status**: ✅ 完成 / Complete

**測試狀態 / Test Status**: ✅ 通過 / Passing

**文件狀態 / Documentation Status**: ✅ 完整 / Complete
