# Football Analysis Qt GUI - User Guide

## 簡介

這是一個完整的足球影片分析系統圖形介面，提供直觀的操作界面來分析足球比賽影片。

## 系統需求

- Python 3.8 或更高版本
- PyQt5
- OpenCV
- NumPy
- Ultralytics YOLO
- 足夠的磁碟空間存放輸出影片

## 安裝步驟

1. 安裝 Python 依賴套件：

```bash
pip install -r requirements.txt
```

2. 準備 YOLO 模型檔案：
   - 將訓練好的 YOLO 模型檔案（.pt 格式）放置在 `models/` 目錄
   - 例如：`models/best.pt`

3. 準備輸入影片：
   - 將要分析的足球比賽影片放置在 `input_videos/` 目錄
   - 支援格式：MP4, AVI, MOV, MKV

## 啟動應用程式

### Linux / macOS

```bash
./run_gui.sh
```

或

```bash
python3 gui.py
```

### Windows

雙擊 `run_gui.bat` 或在命令提示字元中執行：

```batch
python gui.py
```

## 使用說明

### 1. Configuration 標籤（設定）

這是主要的設定介面，您需要在這裡配置分析參數：

#### Input Video（輸入影片）
- 點擊 **Browse...** 按鈕選擇要分析的影片檔案
- 支援的格式：.mp4, .avi, .mov, .mkv
- 確保影片檔案可讀取且格式正確

#### YOLO Model（YOLO 模型）
- 點擊 **Browse...** 按鈕選擇 YOLO 模型檔案
- 必須是 PyTorch 格式的模型檔案（.pt）
- 建議使用針對足球比賽訓練的模型

#### Output Settings（輸出設定）
- **Output Directory**: 指定輸出檔案的目錄（預設：output_videos）
- **Use cached stub files**: 勾選此選項可使用快取檔案加速處理
  - 首次分析建議取消勾選
  - 重複分析相同影片時可勾選以節省時間

#### 控制按鈕
- **Start Analysis**: 開始分析影片
- **Stop Analysis**: 停止正在進行的分析（分析開始後才能使用）

### 2. Analysis Progress 標籤（分析進度）

顯示分析過程的即時資訊：

#### Analysis Progress（分析進度）
- 顯示進度條（不定長度，表示正在處理中）
- 只在分析執行時顯示

#### Analysis Log（分析日誌）
- 即時顯示分析過程的詳細日誌
- 包含資訊、警告和錯誤訊息
- 日誌會以顏色區分不同級別：
  - 黑色：一般資訊
  - 橙色：警告
  - 紅色：錯誤
  - 綠色：成功訊息
- 點擊 **Clear Log** 可清除日誌內容

### 3. Results 標籤（結果）

顯示分析完成後的結果：

#### Output Files（輸出檔案）
- **Video Output**: 顯示輸出影片的完整路徑
  - 點擊 **Open Video** 使用系統預設播放器開啟影片
- **Data Output**: 顯示數據檔案的完整路徑
  - 點擊 **View Data** 載入並顯示數據

#### Analysis Statistics（分析統計）
- **Team Possession**: 顯示各球隊的控球百分比
  - 例如：Team 1: 45.5% | Team 2: 54.5%
- **Player Statistics Table**: 顯示所有球員的統計資料
  - Team: 球隊編號（team_1, team_2）
  - Player ID: 球員識別碼
  - Distance (m): 球員移動的總距離（公尺）

## 輸出檔案說明

分析完成後，會在輸出目錄生成以下檔案：

1. **output_video.avi**
   - 帶有標註的分析影片
   - 包含球員追蹤框、球隊顏色、速度資訊等視覺化元素

2. **data_output.json**
   - JSON 格式的完整分析數據
   - 包含所有球員的統計資料和球隊控球資訊

3. **data_output.csv**
   - CSV 格式的分析數據（表格形式）
   - 可用 Excel 或其他試算表軟體開啟

## 常見問題

### Q: 分析需要多長時間？
A: 取決於影片長度和電腦效能。一般來說：
- 短影片（<5分鐘）：幾分鐘
- 長影片（10-30分鐘）：10-30分鐘
- 使用快取檔案可大幅縮短重複分析時間

### Q: 為什麼無法開啟輸出影片？
A: 請確保：
1. 分析已成功完成
2. 系統已安裝影片播放軟體
3. 輸出檔案未被其他程式佔用

### Q: 「Use cached stub files」選項何時使用？
A: 
- 首次分析：**不要**勾選
- 重複分析相同影片：**勾選**（可節省大量時間）
- 更換模型後：**不要**勾選

### Q: 分析失敗怎麼辦？
A: 檢查以下事項：
1. 影片檔案是否損壞或格式不支援
2. YOLO 模型檔案是否正確
3. 磁碟空間是否充足
4. 查看 Analysis Log 中的錯誤訊息

### Q: 可以同時分析多個影片嗎？
A: 目前版本一次只能分析一個影片。如需分析多個影片，請依序執行。

## 技術支援

如遇到問題，請：
1. 查看 Analysis Log 中的詳細錯誤訊息
2. 確認所有依賴套件已正確安裝
3. 檢查 Python 版本（需要 3.8+）
4. 查看 GitHub Issues 頁面

## 功能特色

- ✅ 直觀的圖形介面
- ✅ 即時進度顯示
- ✅ 詳細的日誌記錄
- ✅ 完整的錯誤處理
- ✅ 結果視覺化展示
- ✅ 一鍵開啟輸出檔案
- ✅ 支援多種影片格式
- ✅ 快取檔案支援
- ✅ 跨平台支援（Windows/Linux/macOS）

## 授權資訊

請參閱專案的 LICENSE 檔案。
