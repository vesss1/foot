# UI Layout and Readability Improvements

## 請求分析

針對目前視窗的「排版/可讀性」做 UI review（不改功能/流程）

---

## 1. 目前排版最影響使用的 5 個問題

### 問題 1: 留白不一致且整體間距控制不佳
**症狀：**
- mainLayout 沒有設定 spacing 和 margins，使用 Qt 預設值（通常較大）
- 各個 GroupBox 之間的間距不統一
- GroupBox 內部的 layout 也沒有統一的 spacing/margins 設定

**影響：**
- 視覺上顯得鬆散、不專業
- 浪費螢幕空間
- 缺乏視覺一致性

### 問題 2: 輸入欄位對齊方式不佳
**症狀：**
- Input Video 和 YOLO Model 使用獨立的 QGroupBox
- 每個 GroupBox 內部使用 QHBoxLayout（LineEdit + Button）
- Label 在 GroupBox 的標題而非欄位左側
- 兩個 GroupBox 佔用過多垂直空間

**影響：**
- 視覺上不夠整齊，不像傳統的表單
- 兩個輸入欄位分開，缺乏群組感
- 垂直空間利用不佳

### 問題 3: Start Button 和 Status Label 群組性不明確
**症狀：**
- Start Button 和 Status Label 直接加入 mainLayout
- 沒有使用 GroupBox 或其他容器組織
- Start Button 設定 minHeight=40，但其他按鈕沒有統一尺寸

**影響：**
- 視覺上顯得散亂，缺乏層次
- 不清楚這兩個元件的關聯性
- 按鈕尺寸不一致

### 問題 4: Analysis Log 固定高度過小且不靈活
**症狀：**
- outputTextEdit->setMinimumHeight(150) 固定最小高度
- addWidget 沒有 stretch factor
- 當有大量輸出時，空間不夠

**影響：**
- 使用者需要頻繁滾動查看日誌
- 空間分配不合理
- 無法根據內容自動調整

### 問題 5: Results 區域和 Log 區域比例失衡
**症狀：**
- Analysis Log 和 Analysis Results 都使用 addWidget() 沒有 stretch
- 沒有明確的空間分配策略
- resultScrollArea->setMinimumHeight(200) 固定高度
- videoWidget->setMinimumHeight(300) 固定高度

**影響：**
- Results 區域（更重要）沒有獲得足夠空間
- 整體資訊密度不平衡
- 固定高度限制了介面彈性

---

## 2. 對應的改法（Qt Widgets 做法描述）

### 改法 1: 統一設定 spacing 和 margins
**實施：**
```cpp
// 主要 Layout
mainLayout->setSpacing(10);           // 元件間距 10px
mainLayout->setContentsMargins(10, 10, 10, 10);  // 外邊距 10px

// 各個 GroupBox 內的 Layout
inputFormLayout->setSpacing(8);
inputFormLayout->setContentsMargins(10, 15, 10, 10);

outputLayout->setSpacing(0);          // TextEdit 不需要內部間距
outputLayout->setContentsMargins(10, 15, 10, 10);

resultsLayout->setSpacing(0);         // TabWidget 佔滿空間
resultsLayout->setContentsMargins(10, 15, 10, 10);
```

**效果：**
- 統一的間距體驗
- 更緊湊但不擁擠的排版
- 專業的視覺呈現

### 改法 2: 使用 QFormLayout 整合輸入欄位
**實施：**
```cpp
// 合併為單一 GroupBox "Input Configuration"
QGroupBox *inputGroup = new QGroupBox("Input Configuration", this);
QFormLayout *inputFormLayout = new QFormLayout(inputGroup);

// Video File 行
QHBoxLayout *videoRowLayout = new QHBoxLayout();
inputVideoPathEdit = new QLineEdit(this);
browseInputButton = new QPushButton("Browse...", this);
browseInputButton->setMinimumWidth(90);  // 統一按鈕寬度
videoRowLayout->addWidget(inputVideoPathEdit, 1);  // stretch=1
videoRowLayout->addWidget(browseInputButton, 0);   // stretch=0
inputFormLayout->addRow("Video File:", videoRowLayout);

// YOLO Model 行
QHBoxLayout *modelRowLayout = new QHBoxLayout();
modelPathEdit = new QLineEdit(this);
browseModelButton = new QPushButton("Browse...", this);
browseModelButton->setMinimumWidth(90);
modelRowLayout->addWidget(modelPathEdit, 1);
modelRowLayout->addWidget(browseModelButton, 0);
inputFormLayout->addRow("YOLO Model:", modelRowLayout);
```

**效果：**
- 標籤和輸入欄位自動對齊
- 減少垂直空間佔用
- 更清晰的表單結構
- Browse 按鈕寬度統一（90px）

### 改法 3: 建立 Control GroupBox 組織按鈕和狀態
**實施：**
```cpp
QGroupBox *controlGroup = new QGroupBox("Analysis Control", this);
QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
controlLayout->setSpacing(8);
controlLayout->setContentsMargins(10, 15, 10, 10);

startButton = new QPushButton("Start Analysis", this);
startButton->setMinimumHeight(40);
controlLayout->addWidget(startButton);

statusLabel = new QLabel("Ready", this);
statusLabel->setStyleSheet("QLabel { padding: 8px; background-color: #f0f0f0; border-radius: 3px; }");
controlLayout->addWidget(statusLabel);
```

**效果：**
- 明確的功能分組
- 更好的視覺層次
- 改善 status label 的 padding（5px -> 8px）並加入 border-radius

### 改法 4: 移除固定高度，使用 stretch factor
**實施：**
```cpp
// 移除 outputTextEdit->setMinimumHeight(150)
outputTextEdit = new QTextEdit(this);
outputTextEdit->setReadOnly(true);
outputLayout->addWidget(outputTextEdit);

// 使用 stretch factor
mainLayout->addWidget(outputGroup, 1);  // stretch=1
```

**效果：**
- Log 區域可以根據視窗大小自動調整
- 更靈活的空間分配
- 改善使用體驗

### 改法 5: 使用 stretch factor 優化空間分配
**實施：**
```cpp
mainLayout->addWidget(inputGroup, 0);      // 固定大小
mainLayout->addWidget(controlGroup, 0);    // 固定大小
mainLayout->addWidget(outputGroup, 1);     // 可伸縮，占 1 份
mainLayout->addWidget(resultsGroupBox, 2); // 可伸縮，占 2 份

// Tab 內容區域
videoLayout->addWidget(videoWidget, 1);    // 視頻佔據主要空間
videoLayout->addLayout(controlsLayout, 0); // 控制按鈕固定高度

// 統一按鈕寬度
playPauseButton->setMinimumWidth(90);
stopButton->setMinimumWidth(90);

// 移除固定高度限制
// 移除 resultScrollArea->setMinimumHeight(200)
// 移除 videoWidget->setMinimumHeight(300)
```

**效果：**
- Results 獲得 2 倍於 Log 的空間（2:1 比例）
- Input 和 Control 保持最小尺寸
- 整體資訊密度更平衡
- 視頻播放器可以充分利用空間

### 額外改進: Tab 內容區域優化
**實施：**
```cpp
// 為每個 Tab 的內容設定統一的 margins 和 spacing
summaryLayout->setContentsMargins(5, 5, 5, 5);
summaryLayout->setSpacing(0);

dataLayout->setContentsMargins(5, 5, 5, 5);
dataLayout->setSpacing(8);

videoLayout->setContentsMargins(5, 5, 5, 5);
videoLayout->setSpacing(8);

// 視頻控制按鈕統一間距
controlsLayout->setSpacing(8);
```

**效果：**
- Tab 內容有適當的留白
- 各 Tab 視覺一致性提升

---

## 3. 改完後的版面結構草圖

### 整體結構（使用 ASCII 描述）

```
╔═══════════════════════════════════════════════════════════════════════╗
║                        Foot Analysis GUI                              ║
║  Window: 1200×900                                                     ║
║  mainLayout: QVBoxLayout (spacing=10, margins=10,10,10,10)           ║
╠═══════════════════════════════════════════════════════════════════════╣
║                                                                       ║
║  ╔═══════════════════════════════════════════════════════════════╗  ║
║  ║  Input Configuration                          [stretch=0]     ║  ║
║  ║  ┌────────────────────────────────────────────────────────┐   ║  ║
║  ║  │ Video File:   [LineEdit: input path    ] [Browse 90px]│   ║  ║
║  ║  │ YOLO Model:   [LineEdit: model path    ] [Browse 90px]│   ║  ║
║  ║  └────────────────────────────────────────────────────────┘   ║  ║
║  ║  QFormLayout (spacing=8, margins=10,15,10,10)                 ║  ║
║  ╚═══════════════════════════════════════════════════════════════╝  ║
║                              ↕ 10px                                   ║
║  ╔═══════════════════════════════════════════════════════════════╗  ║
║  ║  Analysis Control                             [stretch=0]     ║  ║
║  ║  ┌─────────────────────────────────────────────────────────┐  ║  ║
║  ║  │         [Start Analysis Button - 40px height]           │  ║  ║
║  ║  ├─────────────────────────────────────────────────────────┤  ║  ║
║  ║  │  Status: Ready                                          │  ║  ║
║  ║  │  (padding=8px, rounded corners)                         │  ║  ║
║  ║  └─────────────────────────────────────────────────────────┘  ║  ║
║  ║  QVBoxLayout (spacing=8, margins=10,15,10,10)                 ║  ║
║  ╚═══════════════════════════════════════════════════════════════╝  ║
║                              ↕ 10px                                   ║
║  ╔═══════════════════════════════════════════════════════════════╗  ║
║  ║  Analysis Log                                 [stretch=1]     ║  ║
║  ║  ┌───────────────────────────────────────────────────────────┐ ║  ║
║  ║  │ === Analysis Started ===                                  │ ║  ║
║  ║  │ Command: python main.py --input v.mp4 --model best.pt    │ ║  ║
║  ║  │ Reading video...                                          │ ║  ║
║  ║  │ [Real-time output - flexible height]                     │ ║  ║
║  ║  │ [可以根據視窗大小自動調整]                                    │ ║  ║
║  ║  └───────────────────────────────────────────────────────────┘ ║  ║
║  ║  QTextEdit (no minimum height, fills available space)         ║  ║
║  ╚═══════════════════════════════════════════════════════════════╝  ║
║                              ↕ 10px                                   ║
║  ╔═══════════════════════════════════════════════════════════════╗  ║
║  ║  Analysis Results                             [stretch=2]     ║  ║
║  ║  ┌───────────────────────────────────────────────────────────┐ ║  ║
║  ║  │ [Summary] [Data Table] [Video Output]  (TabWidget)       │ ║  ║
║  ║  ├───────────────────────────────────────────────────────────┤ ║  ║
║  ║  │                                                           │ ║  ║
║  ║  │   [Tab Content Area - 2× the height of Log area]         │ ║  ║
║  ║  │   - Summary: ScrollArea with centered message            │ ║  ║
║  ║  │   - Data Table: Table with player stats                  │ ║  ║
║  ║  │   - Video: Video player + controls (Play/Stop 90px)      │ ║  ║
║  ║  │                                                           │ ║  ║
║  ║  │   [更大的顯示區域以便查看結果]                                │ ║  ║
║  ║  │                                                           │ ║  ║
║  ║  └───────────────────────────────────────────────────────────┘ ║  ║
║  ║  QTabWidget with flexible content (no minimum height)         ║  ║
║  ╚═══════════════════════════════════════════════════════════════╝  ║
║                                                                       ║
╚═══════════════════════════════════════════════════════════════════════╝
```

### Layout 樹狀結構

```
MainWindow (1200×900)
└─ centralWidget
   └─ mainLayout: QVBoxLayout
      ├─ spacing: 10px
      ├─ margins: 10, 10, 10, 10
      │
      ├─ [stretch=0] inputGroup: QGroupBox "Input Configuration"
      │  └─ inputFormLayout: QFormLayout
      │     ├─ spacing: 8px
      │     ├─ margins: 10, 15, 10, 10
      │     ├─ Row 1: "Video File:" → QHBoxLayout
      │     │  ├─ inputVideoPathEdit (stretch=1)
      │     │  └─ browseInputButton (stretch=0, minWidth=90)
      │     └─ Row 2: "YOLO Model:" → QHBoxLayout
      │        ├─ modelPathEdit (stretch=1)
      │        └─ browseModelButton (stretch=0, minWidth=90)
      │
      ├─ [stretch=0] controlGroup: QGroupBox "Analysis Control"
      │  └─ controlLayout: QVBoxLayout
      │     ├─ spacing: 8px
      │     ├─ margins: 10, 15, 10, 10
      │     ├─ startButton (minHeight=40)
      │     └─ statusLabel (padding=8, rounded)
      │
      ├─ [stretch=1] outputGroup: QGroupBox "Analysis Log"
      │  └─ outputLayout: QVBoxLayout
      │     ├─ spacing: 0px
      │     ├─ margins: 10, 15, 10, 10
      │     └─ outputTextEdit (no minHeight, flexible)
      │
      └─ [stretch=2] resultsGroupBox: QGroupBox "Analysis Results"
         └─ resultsLayout: QVBoxLayout
            ├─ spacing: 0px
            ├─ margins: 10, 15, 10, 10
            └─ resultsTabWidget: QTabWidget
               ├─ Tab "Summary"
               │  └─ summaryLayout: QVBoxLayout (margins=5,5,5,5)
               │     └─ resultScrollArea → resultImageLabel
               │
               ├─ Tab "Data Table"
               │  └─ dataLayout: QVBoxLayout (margins=5,5,5,5, spacing=8)
               │     ├─ dataLabel
               │     └─ dataTableWidget
               │
               └─ Tab "Video Output"
                  └─ videoLayout: QVBoxLayout (margins=5,5,5,5, spacing=8)
                     ├─ [stretch=1] videoWidget (no minHeight, flexible)
                     └─ [stretch=0] controlsLayout: QHBoxLayout
                        ├─ playPauseButton (minWidth=90)
                        ├─ stopButton (minWidth=90)
                        └─ Stretch (空白填充)
```

### 空間分配示意（垂直方向）

```
總高度: 900px
Margins: 10 + 10 = 20px
Content area: 880px

分配：
┌────────────────────────┐
│ Input Config    ~80px  │  [stretch=0] 固定高度
├────────────────────────┤
│ 10px spacing           │
├────────────────────────┤
│ Control         ~90px  │  [stretch=0] 固定高度
├────────────────────────┤
│ 10px spacing           │
├────────────────────────┤
│ Analysis Log   ~200px  │  [stretch=1] = 200px
│ (flexible)             │  (1/3 of flexible space)
├────────────────────────┤
│ 10px spacing           │
├────────────────────────┤
│ Results        ~400px  │  [stretch=2] = 400px
│ (flexible)             │  (2/3 of flexible space)
└────────────────────────┘

計算：
- 固定部分: 80 + 90 + 30 (3×spacing) = 200px
- 可分配空間: 880 - 200 = 680px
- Log (stretch=1): 680 × 1/3 ≈ 227px
- Results (stretch=2): 680 × 2/3 ≈ 453px
```

---

## 總結

### 主要改進點

1. **統一的間距系統**
   - mainLayout: spacing=10, margins=10
   - GroupBox 內部: spacing=8, margins=(10,15,10,10)
   - Tab 內容: margins=5, spacing=8

2. **改善的表單結構**
   - 使用 QFormLayout 替代多個 QGroupBox
   - Label 和輸入欄位自動對齊
   - 減少垂直空間浪費

3. **明確的功能分組**
   - Input Configuration（輸入配置）
   - Analysis Control（分析控制）
   - Analysis Log（分析日誌）
   - Analysis Results（分析結果）

4. **彈性的空間分配**
   - 移除所有固定高度限制
   - 使用 stretch factor: 0, 0, 1, 2
   - Results 獲得最多空間（2倍）

5. **一致的按鈕尺寸**
   - Browse 按鈕: 90px 寬度
   - Video 控制按鈕: 90px 寬度
   - Start 按鈕: 40px 高度

### 視覺改善

- ✅ 更緊湊但不擁擠的排版
- ✅ 專業的表單對齊
- ✅ 清晰的視覺層次
- ✅ 平衡的資訊密度
- ✅ 靈活的空間利用

### 不變的功能

- ✅ 所有功能保持不變
- ✅ 信號/槽連接不變
- ✅ 業務邏輯不變
- ✅ 只改變排版/可讀性
