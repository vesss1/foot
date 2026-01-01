# Qt Creator UI File 使用指南 / Qt Creator UI File Guide

## 檔案說明 / File Description

- **football_analysis_ui.ui** - Qt Designer/Creator UI 設計檔
- **qt_ui_app_from_designer.py** - 使用 .ui 檔案的 Python 程式

## Qt Creator / Qt Designer 使用方法

### 1. 開啟 UI 檔案 / Open UI File

使用 Qt Designer 或 Qt Creator 開啟：

Open with Qt Designer or Qt Creator:

```bash
# 使用 Qt Designer (如果已安裝)
designer football_analysis_ui.ui

# 或使用 Qt Creator
qtcreator football_analysis_ui.ui
```

### 2. 視覺化編輯 / Visual Editing

在 Qt Designer 中可以：
- 拖放 widgets 來調整佈局
- 修改屬性（顏色、字體、大小等）
- 新增或刪除 UI 元素
- 調整間距和對齊

In Qt Designer you can:
- Drag and drop widgets to adjust layout
- Modify properties (colors, fonts, sizes, etc.)
- Add or remove UI elements
- Adjust spacing and alignment

### 3. 元件名稱對照 / Widget Name Reference

UI 檔案中的元件對應到 Python 程式中的變數名稱：

Widget names in .ui file correspond to Python variables:

| UI 元件名稱 / Widget Name | 用途 / Purpose | Python 存取 / Access |
|---------------------------|----------------|---------------------|
| `videoLabel` | 影片顯示區域 / Video display | `self.videoLabel` |
| `playButton` | 播放/暫停按鈕 / Play/Pause | `self.playButton` |
| `stopButton` | 停止按鈕 / Stop button | `self.stopButton` |
| `loadVideoButton` | 載入影片按鈕 / Load video | `self.loadVideoButton` |
| `positionSlider` | 進度條 / Progress slider | `self.positionSlider` |
| `frameInfoLabel` | 幀資訊 / Frame info | `self.frameInfoLabel` |
| `team1PossessionLabel` | 隊伍1控球率 / Team 1 possession | `self.team1PossessionLabel` |
| `team2PossessionLabel` | 隊伍2控球率 / Team 2 possession | `self.team2PossessionLabel` |
| `totalDistanceLabel` | 總距離 / Total distance | `self.totalDistanceLabel` |
| `avgDistanceLabel` | 平均距離 / Average distance | `self.avgDistanceLabel` |
| `videoDurationLabel` | 影片時長 / Video duration | `self.videoDurationLabel` |
| `avgSpeedLabel` | 平均速度 / Average speed | `self.avgSpeedLabel` |
| `refreshButton` | 重新整理按鈕 / Refresh button | `self.refreshButton` |

## Python 程式使用方法

### 方法 1：使用 .ui 檔案（推薦用於開發）

Method 1: Use .ui file (Recommended for development)

```bash
# 執行使用 .ui 檔案的版本
python qt_ui_app_from_designer.py
```

這個版本會在執行時動態載入 .ui 檔案。

This version dynamically loads the .ui file at runtime.

**優點 / Advantages:**
- 可以即時修改 UI 設計
- 不需要編譯或轉換
- 方便快速原型開發

**缺點 / Disadvantages:**
- 需要 .ui 檔案在執行時可用
- 載入稍慢

### 方法 2：轉換為 Python 程式碼

Method 2: Convert to Python code

```bash
# 使用 pyuic5 將 .ui 檔案轉換為 Python 程式碼
pyuic5 football_analysis_ui.ui -o ui_generated.py

# 然後在你的程式中匯入
# from ui_generated import Ui_FootballAnalysisMainWindow
```

**優點 / Advantages:**
- 不需要 .ui 檔案
- 載入速度稍快
- 適合發佈產品

**缺點 / Disadvantages:**
- 每次修改 UI 都需要重新轉換
- 生成的程式碼不應該手動編輯

### 方法 3：原始程式碼版本（目前使用）

Method 3: Original code version (Currently used)

```bash
# 使用完全用 Python 寫的版本
python qt_ui_app.py
```

這是目前主要使用的版本，不依賴 .ui 檔案。

This is the main version currently in use, doesn't depend on .ui file.

## 如何安裝 Qt Designer

### Linux (Ubuntu/Debian)

```bash
sudo apt-get install qttools5-dev-tools
sudo apt-get install qttools5-dev
```

### macOS

```bash
brew install qt5
# Qt Designer 位於：/usr/local/opt/qt5/bin/Designer.app
```

### Windows

下載並安裝 Qt Creator：
https://www.qt.io/download-qt-installer

Download and install Qt Creator:
https://www.qt.io/download-qt-installer

## UI 檔案結構 / UI File Structure

```xml
<ui version="4.0">
  <class>FootballAnalysisMainWindow</class>
  <widget class="QMainWindow">
    <widget class="QWidget" name="centralwidget">
      <layout class="QHBoxLayout">
        <!-- 左側：影片面板 / Left: Video Panel -->
        <widget name="videoPanel">
          <layout class="QVBoxLayout">
            <widget name="videoLabel"/>      <!-- 影片顯示 -->
            <widget name="playButton"/>      <!-- 播放按鈕 -->
            <widget name="stopButton"/>      <!-- 停止按鈕 -->
            <widget name="positionSlider"/>  <!-- 進度條 -->
          </layout>
        </widget>
        
        <!-- 右側：統計面板 / Right: Statistics Panel -->
        <widget name="statsPanel">
          <layout class="QVBoxLayout">
            <widget name="possessionGroupBox">  <!-- 控球率 -->
            <widget name="distanceGroupBox">     <!-- 距離統計 -->
            <widget name="timeGroupBox">         <!-- 時間統計 -->
          </layout>
        </widget>
      </layout>
    </widget>
  </widget>
</ui>
```

## 自訂 UI / Customizing the UI

### 修改顏色 / Change Colors

在 Qt Designer 中：
1. 選擇要修改的 widget
2. 在屬性面板找到 "styleSheet"
3. 輸入 CSS 樣式，例如：

In Qt Designer:
1. Select the widget to modify
2. Find "styleSheet" in the properties panel
3. Enter CSS styles, for example:

```css
color: #0066cc;
background-color: #f0f0f0;
font-size: 18px;
```

### 調整佈局 / Adjust Layout

1. 右鍵點擊 widget → Layout → [選擇佈局類型]
2. 使用 spacers 來控制間距
3. 調整 stretch factors 來控制相對大小

1. Right-click widget → Layout → [Choose layout type]
2. Use spacers to control spacing
3. Adjust stretch factors to control relative sizes

## 範例：新增統計項目

Example: Adding New Statistics

### 在 Qt Designer 中：

In Qt Designer:

1. 開啟 `football_analysis_ui.ui`
2. 在統計面板中新增一個 QLabel
3. 設定 objectName 為 `newStatLabel`
4. 設定顯示文字為 "--- new stat"
5. 儲存檔案

### 在 Python 程式中：

In Python code:

```python
# 在 qt_ui_app_from_designer.py 中
def update_statistics_display(self):
    # ... 現有程式碼 ...
    
    # 更新新的統計項目
    self.newStatLabel.setText(f"{new_value}")
```

## 測試 UI 檔案

Test the UI file

```bash
# 1. 產生測試資料
python generate_test_data.py

# 2. 執行使用 .ui 檔案的程式
python qt_ui_app_from_designer.py

# 3. 檢查所有功能是否正常
```

## 比較表 / Comparison

| 特性 / Feature | qt_ui_app.py | qt_ui_app_from_designer.py | .ui file |
|---------------|--------------|----------------------------|----------|
| 視覺化編輯 / Visual editing | ❌ | ❌ | ✅ |
| 執行時載入 / Runtime loading | ✅ | ✅ (需要.ui) | - |
| 程式碼維護 / Code maintenance | 困難 / Hard | 容易 / Easy | 容易 / Easy |
| 適合快速原型 / Good for prototyping | ❌ | ✅ | ✅ |
| 適合發佈 / Good for release | ✅ | ✅ | ✅ |

## 建議工作流程 / Recommended Workflow

### 開發階段 / Development

1. 使用 Qt Designer 編輯 `football_analysis_ui.ui`
2. 執行 `qt_ui_app_from_designer.py` 測試變更
3. 反覆修改直到滿意

### 發佈階段 / Release

1. 最終化 .ui 設計
2. 選擇使用 .ui 檔案或轉換為 Python 程式碼
3. 測試所有功能

## 疑難排解 / Troubleshooting

### 問題：找不到 .ui 檔案
**Problem: .ui file not found**

確保 .ui 檔案與 Python 程式在同一目錄。

Ensure .ui file is in the same directory as Python program.

### 問題：pyuic5 不存在
**Problem: pyuic5 not found**

```bash
pip install pyqt5-tools
```

### 問題：Designer 無法開啟
**Problem: Designer won't open**

檢查 Qt Designer 是否已安裝：

Check if Qt Designer is installed:

```bash
# Linux/Mac
which designer

# 或使用 Python 啟動
python -m PyQt5.uic.pyuic
```

## 參考資源 / References

- Qt Designer Manual: https://doc.qt.io/qt-5/qtdesigner-manual.html
- PyQt5 Documentation: https://www.riverbankcomputing.com/static/Docs/PyQt5/
- UI File Format: https://doc.qt.io/qt-5/designer-ui-file-format.html

---

**注意 / Note**: 目前的主程式 `qt_ui_app.py` 不依賴 .ui 檔案，可以獨立運行。.ui 檔案是為了方便未來的 UI 設計和修改而提供的額外工具。

The main program `qt_ui_app.py` doesn't depend on the .ui file and can run independently. The .ui file is provided as an additional tool for future UI design and modifications.
