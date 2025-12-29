# 資料匯出功能指南

## 概述

足球分析系統現在包含全面的資料匯出功能，讓您可以將所有分析結果匯出為結構化檔案格式（JSON 和 CSV）。這使得進一步分析、與其他工具整合以及存檔比賽統計資料成為可能。

## 匯出什麼資料？

系統匯出五種不同類型的資料檔案：

### 1. 球員統計 CSV (`*_player_stats.csv`)
包含逐幀球員資料：
- 球員 ID 和球隊分配
- 邊界框座標 (x1, y1, x2, y2)
- 位置座標 (x, y)
- 速度（公里/小時）
- 累積距離（米）
- 控球指示器（True/False）

**使用案例：**
- 建立球員表現圖表
- 分析移動模式
- 計算時間段內的平均速度
- 追蹤個別球員貢獻

### 2. 足球追蹤 JSON (`*_ball_tracking.json`)
包含逐幀足球資料：
- 每幀的足球位置
- 邊界框座標

**使用案例：**
- 分析足球移動模式
- 追蹤足球軌跡
- 計算足球速度和距離

### 3. 球隊控球 JSON (`*_team_possession.json`)
包含球隊控球統計：
- 每支球隊的控球幀數
- 控球百分比
- 逐幀控球記錄

**使用案例：**
- 比較球隊優勢
- 分析隨時間變化的控球趨勢
- 識別控球連續性

### 4. 相機移動 CSV (`*_camera_movement.csv`)
包含逐幀相機移動：
- X 軸移動（像素）
- Y 軸移動（像素）

**使用案例：**
- 分析相機操作員行為
- 在自訂分析中補償相機移動
- 追蹤比賽期間的焦點區域

### 5. 綜合摘要 JSON (`*_summary.json`)
包含彙總統計資料：
- 球員摘要（最高速度、總距離、控球時間）
- 球隊控球摘要
- 足球偵測統計
- 相機移動總計
- 分析中繼資料（時間戳記、總幀數）

**使用案例：**
- 快速瀏覽比賽統計
- 生成報告
- 比較多場比賽

## 如何使用

### 選項 1：圖形介面應用程式（最簡單）

1. 啟動 Qt 圖形介面：
```bash
python qt_main.py
```

2. 選擇您的輸入影片
3. 啟用「匯出分析資料（JSON/CSV）」核取方塊（預設啟用）
4. 點擊「開始分析」
5. 在輸出目錄中尋找匯出的檔案

### 選項 2：命令列（最靈活）

```bash
# 基本用法（匯出到 output_videos/）
python main.py --input my_video.mp4

# 自訂匯出目錄
python main.py --input my_video.mp4 --export-dir results/game1

# 自訂檔案名稱前綴
python main.py --input my_video.mp4 --export-prefix match_2024_01_15

# 停用匯出
python main.py --input my_video.mp4 --no-export

# 完全自訂
python main.py \
  --input videos/game.mp4 \
  --output results/annotated.avi \
  --export-dir results/data \
  --export-prefix game_analysis
```

## 分析匯出的資料

### 使用提供的範例腳本

```bash
# 分析預設位置的資料
python analyze_exported_data.py

# 分析自訂位置的資料
python analyze_exported_data.py results/data
```

此腳本展示：
- 載入和解析所有匯出的檔案
- 計算額外的統計資料
- 尋找控球連續性
- 顯示全面的比賽分析

### 使用 Python (Pandas)

```python
import pandas as pd
import json

# 載入球員統計
df = pd.read_csv('output_videos/analysis_player_stats_TIMESTAMP.csv')

# 篩選特定球員的資料
player_1 = df[df['player_id'] == 1]

# 計算平均速度
avg_speed = player_1['speed_kmh'].mean()
print(f"球員 1 平均速度：{avg_speed:.2f} 公里/小時")

# 繪製球員距離隨時間變化圖
import matplotlib.pyplot as plt
plt.plot(player_1['frame'], player_1['distance_m'])
plt.xlabel('幀')
plt.ylabel('距離（米）')
plt.title('球員 1 距離隨時間變化')
plt.show()
```

### 使用 Python (JSON)

```python
import json

# 載入綜合摘要
with open('output_videos/analysis_summary_TIMESTAMP.json', 'r') as f:
    summary = json.load(f)

# 存取球隊控球
team1_pct = summary['team_possession']['team_1']['percentage']
team2_pct = summary['team_possession']['team_2']['percentage']

print(f"球隊 1：{team1_pct}% 控球率")
print(f"球隊 2：{team2_pct}% 控球率")

# 存取球員統計
for player_id, stats in summary['player_summary'].items():
    print(f"球員 {player_id}：{stats['max_speed_kmh']:.1f} 公里/小時 最高速度")
```

### 使用 Excel/Google Sheets

1. 直接在 Excel 或 Google Sheets 中開啟 CSV 檔案
2. 建立樞紐分析表以彙總資料
3. 生成圖表和圖形
4. 按不同欄位篩選和排序

### 使用資料分析工具

匯出的格式相容於：
- **R**：使用 `read.csv()` 和 `jsonlite::fromJSON()`
- **MATLAB**：使用 `readtable()` 和 `jsondecode()`
- **Tableau**：直接匯入 CSV 檔案
- **Power BI**：直接匯入 CSV 檔案
- **SQL 資料庫**：將 CSV 檔案載入資料庫表格

## 檔案命名規則

所有匯出的檔案遵循此命名模式：
```
{前綴}_{類型}_{時間戳記}.{副檔名}

範例：
- analysis_player_stats_20240115_143022.csv
- match1_summary_20240115_143022.json
- game_team_possession_20240115_143022.json
```

其中：
- `前綴`：可自訂前綴（預設：「analysis」）
- `類型`：檔案類型（player_stats、ball_tracking 等）
- `時間戳記`：匯出的日期和時間（YYYYMMDD_HHMMSS）
- `副檔名`：檔案格式（csv 或 json）

## 提示和最佳實踐

### 儲存
- 匯出的檔案對於 5 分鐘影片通常為 10-50 KB
- 按比賽/日期將檔案儲存在有組織的目錄中
- 保留摘要檔案以便快速參考

### 分析
- 使用摘要檔案獲取快速見解
- 使用詳細的 CSV 檔案進行深入分析
- 結合多場比賽的匯出以追蹤隨時間變化的趨勢

### 自動化
- 在腳本中使用命令列參數
- 批次處理多個影片
- 自動將匯出資料存檔到雲端儲存

### 效能
- 匯出增加的處理時間極少（< 1 秒）
- 對影片處理速度無影響
- 在影片渲染完成後寫入檔案

## 疑難排解

### 檔案未建立
- 檢查是否啟用匯出（未使用 `--no-export`）
- 驗證輸出目錄具有寫入權限
- 檢查可用磁碟空間

### 無法開啟檔案
- 使用線上驗證器確保 JSON 檔案有效
- 使用 UTF-8 編碼開啟 CSV 檔案
- 某些編輯器可能需要明確選擇編碼

### 資料遺失
- 如果資料不可用，某些欄位可能為空（例如，沒有控球）
- 檢查原始影片分析是否有問題
- 驗證所有分析步驟是否成功完成

## 進階用法

### 自訂後處理

建立自訂分析腳本：
- 比較多場比賽的球員表現
- 生成自動化報告
- 計算自訂指標
- 建立視覺化和熱圖

### 與其他工具整合

- 匯入運動分析軟體
- 將資料饋送到機器學習模型
- 建立即時儀表板
- 建立球員表現資料庫

## 範例

查看 `analyze_exported_data.py` 腳本以獲取完整範例：
- 載入所有匯出的檔案
- 計算額外的統計資料
- 顯示格式化輸出
- 展示常見的分析模式

## 支援

如有問題或疑問：
1. 查看本指南以獲取常見解決方案
2. 查閱範例腳本以獲取使用模式
3. 確保所有檔案都已成功匯出
4. 檢查檔案權限和磁碟空間

---

**祝分析愉快！** 📊⚽
