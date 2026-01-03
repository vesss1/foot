# UI Review 快速參考 (Quick Reference)

## 請求內容
針對目前視窗的「排版/可讀性」做 UI review（不要改功能/流程）

---

## 1. 目前排版最影響使用的 5 個問題

| # | 問題 | 影響 |
|---|------|------|
| 1 | **留白不一致** | 視覺上鬆散、不專業、浪費空間 |
| 2 | **輸入欄位對齊不佳** | 不夠整齊、缺乏群組感、垂直空間利用不佳 |
| 3 | **控制元件群組性不明確** | 顯得散亂、缺乏層次、按鈕尺寸不一致 |
| 4 | **Log 固定高度過小** | 需頻繁滾動、空間分配不合理 |
| 5 | **空間分配失衡** | Results（更重要）沒獲得足夠空間 |

---

## 2. 對應的改法

### 改法 1: 統一 spacing 和 margins
```cpp
mainLayout->setSpacing(10);
mainLayout->setContentsMargins(10, 10, 10, 10);
inputFormLayout->setSpacing(8);
inputFormLayout->setContentsMargins(10, 15, 10, 10);
// 其他 layout 同樣設定
```

### 改法 2: 使用 QFormLayout
```cpp
QGroupBox *inputGroup = new QGroupBox("Input Configuration", this);
QFormLayout *inputFormLayout = new QFormLayout(inputGroup);
inputFormLayout->addRow("Video File:", videoRowLayout);
inputFormLayout->addRow("YOLO Model:", modelRowLayout);
```

### 改法 3: 建立 Control GroupBox
```cpp
QGroupBox *controlGroup = new QGroupBox("Analysis Control", this);
QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
controlLayout->addWidget(startButton);
controlLayout->addWidget(statusLabel);
```

### 改法 4: 移除固定高度 + stretch
```cpp
// 移除: outputTextEdit->setMinimumHeight(150);
mainLayout->addWidget(outputGroup, 1);  // stretch=1
```

### 改法 5: 使用 stretch factor
```cpp
mainLayout->addWidget(inputGroup, 0);      // 固定
mainLayout->addWidget(controlGroup, 0);    // 固定
mainLayout->addWidget(outputGroup, 1);     // 1 份
mainLayout->addWidget(resultsGroupBox, 2); // 2 份（Results 獲得 2× 空間）
```

---

## 3. 改完後的版面結構

```
╔═══════════════════════════════════════════╗
║         Foot Analysis GUI (1200×900)      ║
╠═══════════════════════════════════════════╣
║ [Input Configuration]         [stretch=0] ║  ~80px
║   Video File:  [_____] [Browse 90px]      ║
║   YOLO Model:  [_____] [Browse 90px]      ║
╠═══════════════════════════════════════════╣
║ [Analysis Control]            [stretch=0] ║  ~90px
║   [Start Analysis - 40px]                 ║
║   Status: Ready (圓角)                     ║
╠═══════════════════════════════════════════╣
║ [Analysis Log]                [stretch=1] ║  ~227px
║   [彈性高度，可調整]                        ║
║                                           ║
╠═══════════════════════════════════════════╣
║ [Analysis Results]            [stretch=2] ║  ~453px
║   [Summary|Data Table|Video Output]       ║
║                                           ║
║   [2× Log 空間，顯示結果]                   ║
║                                           ║
╚═══════════════════════════════════════════╝

spacing: 10px between sections
```

---

## 關鍵數值

### Spacing (間距)
- 主要區塊間: **10px**
- GroupBox 內部: **8px**
- Tab 內容: **8px**

### Margins (邊距)
- 主視窗: **(10, 10, 10, 10)**
- GroupBox: **(10, 15, 10, 10)**
- Tab 內容: **(5, 5, 5, 5)**

### Stretch Factors (伸縮因子)
- Input: **0** (固定)
- Control: **0** (固定)
- Log: **1** (1 份彈性空間)
- Results: **2** (2 份彈性空間)

### Button Sizes (按鈕尺寸)
- Browse 按鈕寬度: **90px**
- Play/Stop 按鈕寬度: **90px**
- Start 按鈕高度: **40px**

---

## 改善效果

### Before → After

**輸入區域:**
```
[Input Video GroupBox]     →  [Input Configuration]
[YOLO Model GroupBox]          Video File:  [__][90px]
                               YOLO Model:  [__][90px]
(2 個 GroupBox)                (1 個 GroupBox + QFormLayout)
```

**控制區域:**
```
[Start Button]             →  [Analysis Control]
Status: Ready                 [Start Button - 40px]
                              Status: Ready (圓角, 8px padding)
(獨立元件)                     (群組在一起)
```

**空間分配:**
```
Log:     ████████           →  Log:     ████████
Results: ████████               Results: ████████████████
(1:1 比例)                      (1:2 比例，Results 獲得 2× 空間)
```

---

## 檔案變更

### 修改的檔案
1. **MainWindow.h** - 加入 `#include <QFormLayout>`
2. **MainWindow.cpp** - setupUI() 函數重構

### 新增的檔案
1. **UI_LAYOUT_IMPROVEMENTS.md** - 詳細分析和解決方案
2. **UI_IMPROVEMENT_SUMMARY.md** - 前後對比表格
3. **UI_REVIEW_QUICK_REFERENCE.md** - 快速參考（本文件）

---

## 總結

✅ **5 個主要問題全部解決**
✅ **使用標準 Qt Widgets 技術**
✅ **沒有改變任何功能或流程**
✅ **更專業、更易用、更美觀**

所有改變都是**純 UI/排版改善**，可以安全編譯和執行。
