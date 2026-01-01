# Football Analysis System

足球影片分析系統，使用 YOLO 進行物體追蹤、球隊分配、速度與距離估算等功能。

## 功能特點

- **物體追蹤**: 使用 YOLO 模型追蹤球員、足球和裁判
- **球隊分配**: 自動識別並分配球員到不同球隊
- **相機移動估算**: 補償相機移動以獲得準確的位置數據
- **速度與距離計算**: 計算每個球員的移動速度和距離
- **球權分析**: 分析各球隊的控球百分比
- **視覺化輸出**: 生成帶有註釋的分析影片
- **數據輸出**: 輸出 JSON 和 CSV 格式的分析數據

## Qt 圖形介面

本系統提供了完整的 Qt 圖形介面，方便使用者操作：

### 功能特色

1. **輸入/配置頁面**
   - 選擇輸入影片檔案
   - 選擇 YOLO 模型檔案
   - 設定輸出目錄
   - 使用快取檔案選項（加速處理）

2. **分析進度監控**
   - 即時進度顯示
   - 詳細日誌查看
   - 開始/停止分析控制

3. **結果展示**
   - 輸出影片路徑顯示與開啟
   - 分析數據檢視
   - 球隊控球統計
   - 球員距離統計表格

### 使用方法

#### 安裝依賴

```bash
cd foot-Function
pip install -r requirements.txt
```

#### 啟動 GUI（Linux/Mac）

```bash
./run_gui.sh
```

或直接執行：

```bash
python3 gui.py
```

#### 啟動 GUI（Windows）

```batch
run_gui.bat
```

或直接執行：

```batch
python gui.py
```

### 使用步驟

1. 啟動 GUI 應用程式
2. 在「Configuration」標籤中：
   - 點擊「Browse...」選擇輸入影片
   - 點擊「Browse...」選擇 YOLO 模型檔案（.pt）
   - 設定輸出目錄（預設為 output_videos）
   - 選擇是否使用快取檔案
3. 點擊「Start Analysis」開始分析
4. 在「Analysis Progress」標籤查看進度和日誌
5. 分析完成後，在「Results」標籤查看結果：
   - 點擊「Open Video」開啟輸出影片
   - 點擊「View Data」檢視數據
   - 查看球隊控球統計和球員距離統計

## 命令列使用

如果您偏好使用命令列，可以直接執行主程式：

```bash
cd foot-Function
python3 main.py
```

## 系統需求

- Python 3.8+
- OpenCV
- NumPy
- Ultralytics YOLO
- PyQt5（用於 GUI）
- 其他依賴請參考 requirements.txt

## 專案結構

```
foot-Function/
├── gui.py                          # Qt 圖形介面主程式
├── main.py                         # 命令列主程式
├── run_gui.sh                      # Linux/Mac 啟動腳本
├── run_gui.bat                     # Windows 啟動腳本
├── requirements.txt                # Python 依賴套件
├── camera_movement_estimator/      # 相機移動估算模組
├── player_ball_assigner/           # 球員球權分配模組
├── speed_and_distance_estimator/   # 速度與距離計算模組
├── team_assigner/                  # 球隊分配模組
├── trackers/                       # 物體追蹤模組
├── view_transformer/               # 視角轉換模組
├── utils/                          # 工具函數
├── models/                         # YOLO 模型檔案目錄
├── input_videos/                   # 輸入影片目錄
├── output_videos/                  # 輸出影片和數據目錄
└── stubs/                          # 快取檔案目錄
```

## 輸出檔案

- `output_videos/output_video.avi`: 帶有註釋的分析影片
- `output_videos/data_output.json`: JSON 格式的分析數據
- `output_videos/data_output.csv`: CSV 格式的分析數據

## 注意事項

- 首次分析時建議取消勾選「使用快取檔案」選項
- 確保 YOLO 模型檔案（best.pt）已放置在 models/ 目錄
- 確保輸入影片已放置在 input_videos/ 目錄
- 分析大型影片可能需要較長時間，請耐心等待

## 授權

請參閱專案授權檔案
