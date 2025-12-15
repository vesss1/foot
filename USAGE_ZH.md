# 使用指南 - 足球分析系統

本指南將詳細教您如何使用足球分析系統來分析足球比賽影片。

## 目錄
1. [快速開始](#快速開始)
2. [詳細安裝步驟](#詳細安裝步驟)
3. [使用圖形介面（推薦）](#使用圖形介面推薦)
4. [使用命令列](#使用命令列)
5. [理解輸出結果](#理解輸出結果)
6. [進階配置](#進階配置)
7. [疑難排解](#疑難排解)
8. [實際案例](#實際案例)

---

## 快速開始

### 最快速的方法（5 個步驟）

```bash
# 1. 複製專案
git clone https://github.com/vesss1/foot.git
cd foot

# 2. 安裝依賴
pip install -r requirements.txt

# 3. 下載模型（請從連結手動下載）
# https://drive.google.com/file/d/1DC2kCygbBWUKheQ_9cFziCsYVSRw6axK/view?usp=sharing
# 將下載的 best.pt 放到 models/ 資料夾

# 4. 下載範例影片（選擇性）
# https://drive.google.com/file/d/1t6agoqggZKx6thamUuPAIdN_1zR9v9S_/view?usp=sharing
# 將影片放到 input_videos/ 資料夾

# 5. 啟動圖形介面
python qt_main.py
```

---

## 詳細安裝步驟

### 步驟 1：確認 Python 版本

```bash
# 檢查 Python 版本（需要 3.7 或更高）
python --version
# 或
python3 --version
```

如果沒有安裝 Python 3.7+，請從 [python.org](https://www.python.org/downloads/) 下載並安裝。

### 步驟 2：複製儲存庫

```bash
# 使用 Git 複製
git clone https://github.com/vesss1/foot.git

# 進入專案目錄
cd foot

# 查看專案結構
ls -la
```

### 步驟 3：安裝依賴套件

#### 方法 A：使用 requirements.txt（推薦）

```bash
pip install -r requirements.txt
```

#### 方法 B：手動安裝每個套件

```bash
pip install ultralytics
pip install supervision
pip install opencv-python
pip install numpy
pip install matplotlib
pip install pandas
pip install scikit-learn
pip install PyQt5
```

#### 驗證安裝

```bash
# 檢查是否所有套件都已安裝
python -c "import ultralytics, supervision, cv2, numpy, matplotlib, pandas, sklearn, PyQt5; print('所有套件已成功安裝！')"
```

### 步驟 4：下載訓練的 YOLO 模型

1. 訪問：https://drive.google.com/file/d/1DC2kCygbBWUKheQ_9cFziCsYVSRw6axK/view?usp=sharing
2. 下載 `best.pt` 檔案
3. 將檔案放在專案的 `models/` 目錄中

```bash
# 確認模型檔案位置
ls -lh models/best.pt
```

### 步驟 5：準備輸入影片

您可以：
- 下載範例影片：https://drive.google.com/file/d/1t6agoqggZKx6thamUuPAIdN_1zR9v9S_/view?usp=sharing
- 或使用您自己的足球比賽影片

將影片放在 `input_videos/` 目錄中。

---

## 使用圖形介面（推薦）

圖形介面是最簡單的使用方式，特別適合初學者。

### 啟動圖形介面

```bash
python qt_main.py
```

### 圖形介面詳細操作

#### 1. 選擇輸入影片

![選擇影片]
1. 點擊視窗左上方的「**瀏覽...**」按鈕
2. 在檔案瀏覽器中找到您的影片檔案
3. 選擇影片並點擊「開啟」
4. 影片的第一幀將顯示在右側的預覽區

**支援的影片格式：**
- MP4 (.mp4)
- AVI (.avi)
- MOV (.mov)
- 其他常見格式

#### 2. 配置設定（選擇性）

**使用快取資料：**
- ✅ 勾選：使用之前的分析結果（更快，適合重新分析相同影片）
- ❌ 不勾選：完整重新分析（較慢，但使用最新資料）

**模型選擇：**
- 預設：`models/best.pt`（推薦）
- 自訂：點擊「自訂...」選擇其他 YOLO 模型

**輸出路徑：**
- 預設：`output_videos/output_video.avi`
- 點擊「變更...」可選擇不同的輸出位置和檔案名稱

#### 3. 開始分析

1. 點擊綠色的「**開始分析**」按鈕
2. 分析將在後台開始執行
3. 您可以在以下位置看到進度：
   - **進度條**：顯示 0-100% 的完成度
   - **狀態標籤**：顯示當前處理步驟
   - **分析日誌**：顯示詳細的處理資訊

#### 4. 監控進度

分析過程包括以下階段：

| 階段 | 進度 | 說明 |
|------|------|------|
| 讀取影片 | 5% | 載入影片檔案和影格 |
| 初始化追蹤器 | 10% | 載入 YOLO 模型 |
| 偵測和追蹤物體 | 15% | 偵測球員、足球、裁判 |
| 計算位置 | 25% | 計算物體位置 |
| 估計相機移動 | 30% | 分析相機移動 |
| 應用透視轉換 | 40% | 轉換為真實世界座標 |
| 插值足球位置 | 45% | 填補足球偵測間隙 |
| 計算速度和距離 | 50% | 計算球員移動指標 |
| 分配球隊 | 55% | 識別球隊顏色 |
| 分析控球 | 60% | 追蹤控球情況 |
| 生成標註影格 | 70% | 繪製視覺化 |
| 儲存輸出影片 | 90% | 寫入最終影片 |
| 完成 | 100% | 分析完成！ |

**提示：**
- 第一次分析可能需要 5-15 分鐘，取決於影片長度和電腦性能
- 如果啟用快取，後續分析會快得多（1-3 分鐘）

#### 5. 查看結果

分析完成後：
1. 輸出影片將自動載入到右側的播放器
2. 使用播放控制：
   - **播放/暫停按鈕**：控制播放
   - **滑桿**：拖動到任何影格
   - **影格計數器**：顯示當前影格編號

#### 6. 儲存和分享

- 輸出影片已自動儲存到指定路徑
- 預設位置：`output_videos/output_video.avi`
- 您可以直接分享這個檔案

---

## 使用命令列

命令列適合自動化和批次處理。

### 基本使用

```bash
python main.py
```

這將：
1. 讀取 `input_videos/08fd33_4.mp4`
2. 處理影片
3. 輸出到 `output_videos/output_video.avi`

### 自訂輸入和輸出

編輯 `main.py` 檔案：

```python
# 找到這一行（第 14 行）
video_frames = read_video('input_videos/08fd33_4.mp4')

# 改為您的影片路徑
video_frames = read_video('input_videos/您的影片.mp4')

# 找到這一行（第 84 行）
save_video(output_video_frames, 'output_videos/output_video.avi')

# 改為您想要的輸出路徑
save_video(output_video_frames, 'output_videos/我的分析結果.avi')
```

### 啟用/停用快取

在 `main.py` 中：

```python
# 使用快取（更快）
tracks = tracker.get_object_tracks(video_frames,
                                   read_from_stub=True,  # 改為 True
                                   stub_path='stubs/track_stubs.pkl')

# 不使用快取（總是重新分析）
tracks = tracker.get_object_tracks(video_frames,
                                   read_from_stub=False,  # 改為 False
                                   stub_path='stubs/track_stubs.pkl')
```

### 批次處理多個影片

建立一個批次處理腳本 `batch_process.py`：

```python
from utils import read_video, save_video
from trackers import Tracker
# ... 其他匯入 ...

def process_video(input_path, output_path):
    video_frames = read_video(input_path)
    # ... 其餘的處理代碼 ...
    save_video(output_video_frames, output_path)

# 處理多個影片
videos = [
    ('input_videos/match1.mp4', 'output_videos/match1_analysis.avi'),
    ('input_videos/match2.mp4', 'output_videos/match2_analysis.avi'),
    ('input_videos/match3.mp4', 'output_videos/match3_analysis.avi'),
]

for input_path, output_path in videos:
    print(f"處理中: {input_path}")
    process_video(input_path, output_path)
    print(f"完成: {output_path}")
```

---

## 理解輸出結果

分析完成後的影片包含多種視覺化元素：

### 1. 球員追蹤

**橢圓形標記：**
- 每個球員周圍都有一個彩色橢圓
- 顏色代表球隊：
  - **顏色 A**：球隊 1
  - **顏色 B**：球隊 2
- 橢圓跟隨球員移動

**球員 ID：**
- 每個球員有唯一的編號
- 編號顯示在橢圓內或旁邊
- ID 在整個影片中保持一致

### 2. 足球追蹤

**綠色三角形：**
- 標記足球的位置
- 即使在某些影格中偵測不到足球，系統也會插值其位置

### 3. 控球指示

**紅色三角形：**
- 顯示在控球球員上方
- 即時更新控球變化

### 4. 球隊統計

**螢幕左上角顯示：**
- **球隊 1 控球百分比**：球隊 1 的控球時間百分比
- **球隊 2 控球百分比**：球隊 2 的控球時間百分比
- 百分比加總為 100%

### 5. 球員速度和距離

**每個球員顯示：**
- **速度**：當前速度（公里/小時）
- **距離**：累積行進距離（米）
- 資料即時更新

### 6. 相機移動

**螢幕顯示：**
- **Camera Movement X**：水平相機移動
- **Camera Movement Y**：垂直相機移動
- 用於補償相機平移

---

## 進階配置

### 自訂 YOLO 模型

如果您訓練了自己的模型：

```python
# 在 main.py 中（第 17 行）
tracker = Tracker('models/best.pt')

# 改為您的模型路徑
tracker = Tracker('models/my_custom_model.pt')
```

### 調整參數

#### 控球距離閾值

在 `player_ball_assigner/player_ball_assigner.py` 中：

```python
class PlayerBallAssigner:
    def __init__(self):
        self.max_player_ball_distance = 70  # 預設 70 像素
        
# 改為更大或更小的值來調整控球敏感度
```

#### 球隊顏色聚類

在 `team_assigner/team_assigner.py` 中：

```python
def get_clustering_model(self, image):
    image_2d = image.reshape(-1, 3)
    kmeans = KMeans(n_clusters=2, init="k-means++", n_init=1)  # 2 個聚類 = 2 支球隊
```

#### 速度計算窗口

在 `speed_and_distance_estimator/speed_and_distance_estimator.py` 中：

```python
def add_speed_and_distance_to_tracks(self, tracks):
    total_distance = {}
    for object, object_tracks in tracks.items():
        # ...
        for frame_num in range(0, len(object_tracks), 5):  # 每 5 幀計算一次
```

### 視角轉換校準

如果您想分析不同的足球場，需要調整透視轉換點。

在 `view_transformer/view_transformer.py` 中：

```python
def __init__(self):
    court_width = 68  # 米
    court_length = 23.32  # 米
    
    self.pixel_vertices = np.array([
        [110, 1035],  # 左下
        [265, 275],   # 左上
        [910, 260],   # 右上
        [1640, 915]   # 右下
    ])
    
    # 調整這些值以匹配您的影片視角
```

---

## 疑難排解

### 常見問題和解決方案

#### 問題 1：`ModuleNotFoundError: No module named 'XXX'`

**解決方案：**
```bash
# 重新安裝所有依賴
pip install -r requirements.txt

# 或安裝特定的缺失模組
pip install XXX
```

#### 問題 2：模型檔案找不到

**錯誤訊息：**
```
FileNotFoundError: [Errno 2] No such file or directory: 'models/best.pt'
```

**解決方案：**
1. 確認您已下載模型檔案
2. 確認檔案位於 `models/best.pt`
3. 檢查檔案名稱是否正確（不是 `best.pt.download` 或其他）

```bash
# 檢查模型檔案
ls -lh models/best.pt
```

#### 問題 3：記憶體不足

**錯誤訊息：**
```
MemoryError: Unable to allocate...
```

**解決方案：**
1. 處理較短的影片片段
2. 關閉其他應用程式釋放記憶體
3. 減少批次大小（在 `tracker.py` 中修改）

```python
# 在 trackers/tracker.py 中
def detect_frames(self, frames):
    batch_size = 20  # 減少這個值，例如改為 10
```

#### 問題 4：影片無法播放

**解決方案：**
1. 確認影片格式支援（MP4、AVI、MOV）
2. 嘗試使用其他影片播放器（VLC、Windows Media Player）
3. 轉換影片格式：

```bash
# 使用 ffmpeg 轉換
ffmpeg -i input_video.mp4 -c:v libx264 output_video.mp4
```

#### 問題 5：分析速度很慢

**解決方案：**
1. 啟用快取（`read_from_stub=True`）
2. 使用 GPU 加速（如果有 NVIDIA GPU）：
```bash
# 安裝 GPU 版本的 PyTorch
pip install torch torchvision --index-url https://download.pytorch.org/whl/cu118
```
3. 減少影片解析度或幀率

#### 問題 6：球隊顏色識別錯誤

**解決方案：**
1. 確保影片品質良好
2. 確保兩支球隊的球衣顏色有明顯差異
3. 調整 K-means 參數（進階）
4. 手動標註第一幀的球隊（需要修改程式碼）

#### 問題 7：Qt GUI 無法啟動

**錯誤訊息：**
```
ModuleNotFoundError: No module named 'PyQt5'
```

**解決方案：**
```bash
# 安裝 PyQt5
pip install PyQt5

# 如果在 Linux 上，可能還需要：
sudo apt-get install python3-pyqt5
```

---

## 實際案例

### 案例 1：分析一場完整比賽

**目標：**分析一場 90 分鐘的足球比賽

**步驟：**
1. 準備完整比賽影片（建議先轉換為 MP4 格式）
2. 將影片放在 `input_videos/` 中
3. 啟動圖形介面：`python qt_main.py`
4. 選擇影片並開始分析
5. 等待處理完成（可能需要 30-60 分鐘）
6. 查看輸出影片

**注意：**
- 長影片需要大量記憶體和時間
- 建議將影片分成上下半場分別處理

### 案例 2：比較兩支球隊的表現

**目標：**分析多場比賽並比較球隊統計

**步驟：**
1. 分析多個影片：
```bash
python qt_main.py
# 選擇 match1.mp4，分析
# 選擇 match2.mp4，分析
# 選擇 match3.mp4，分析
```

2. 從輸出影片中記錄統計資料：
   - 控球百分比
   - 球員平均速度
   - 球員移動距離

3. 手動建立比較表格

### 案例 3：訓練用途 - 分析球員表現

**目標：**追蹤特定球員在比賽中的表現

**步驟：**
1. 分析比賽影片
2. 在輸出影片中找到特定球員的 ID
3. 觀察該球員的：
   - 移動路徑
   - 速度變化
   - 控球時間
   - 覆蓋的距離

4. 用於訓練回饋和改進

### 案例 4：裁判分析 - 控球時間統計

**目標：**驗證比賽中的控球時間

**步驟：**
1. 分析完整比賽
2. 查看螢幕上的控球百分比統計
3. 驗證是否與官方統計一致
4. 用於裁判培訓或爭議解決

### 案例 5：戰術分析 - 球員移動模式

**目標：**分析球隊的戰術移動

**步驟：**
1. 分析比賽影片
2. 觀察球員的移動距離和速度
3. 識別高活動區域
4. 分析球員位置和陣型

---

## 效能優化建議

### 1. 首次執行優化

```python
# 首次執行：停用快取，生成新資料
read_from_stub=False
```

### 2. 後續執行優化

```python
# 後續執行：啟用快取，使用已生成的資料
read_from_stub=True
```

### 3. 硬體加速

如果您有 NVIDIA GPU：
```bash
# 檢查 CUDA 是否可用
python -c "import torch; print(torch.cuda.is_available())"

# 如果輸出 True，YOLO 將自動使用 GPU
```

### 4. 影片預處理

```bash
# 降低解析度以加快處理
ffmpeg -i input.mp4 -vf scale=1280:720 input_720p.mp4

# 降低幀率
ffmpeg -i input.mp4 -r 24 input_24fps.mp4
```

---

## 取得幫助

如果您遇到本指南未涵蓋的問題：

1. **查看錯誤訊息**：仔細閱讀錯誤訊息，通常會指出問題所在
2. **檢查日誌**：在圖形介面中查看詳細的分析日誌
3. **驗證環境**：確保所有依賴都已正確安裝
4. **查閱文件**：閱讀 README_ZH.md 了解技術細節
5. **查看原始碼**：如果需要進階自訂，查看各模組的原始碼

---

## 下一步

現在您已經知道如何使用這個系統了！嘗試：

1. ✅ 分析您的第一個影片
2. ✅ 實驗不同的設定
3. ✅ 比較多個比賽的結果
4. ✅ 自訂參數以適應您的需求
5. ✅ 分享您的分析結果

**祝您分析愉快！⚽📊**
