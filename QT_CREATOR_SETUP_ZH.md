# Qt Creator 整合設定指南

## 概述

本指南將詳細說明如何將此足球分析專案與 Qt Creator IDE 整合使用。雖然這是一個 Python/PyQt5 專案而非 C++ 專案，但 Qt Creator 提供了優秀的專案管理、代碼導航和編輯功能，使其成為開發和維護此專案的理想選擇。

## 為什麼使用 Qt Creator？

### 優點

1. **專案結構視覺化**
   - 清晰的檔案樹狀結構
   - 按模組組織的原始檔案
   - 快速訪問所有專案檔案

2. **強大的代碼編輯功能**
   - 語法高亮顯示（支援 Python）
   - 代碼自動完成
   - 快速搜尋和導航
   - 多檔案編輯

3. **整合式終端機**
   - 直接在 IDE 內執行 Python 腳本
   - 無需切換視窗
   - 輕鬆監控輸出和錯誤

4. **版本控制整合**
   - 內建 Git 支援
   - 視覺化差異比較
   - 輕鬆提交和推送更改

5. **多平台支援**
   - 在 Windows、macOS 和 Linux 上一致的體驗
   - 跨平台專案管理

## 系統需求

### 必需軟體

1. **Qt Creator**
   - 版本：6.0 或更高版本（建議最新版本）
   - 下載：[Qt Creator 官方網站](https://www.qt.io/product/development-tools)
   - 注意：您可以單獨安裝 Qt Creator，無需完整的 Qt SDK

2. **Python**
   - 版本：3.7 或更高版本
   - 包含 pip 套件管理器

3. **專案依賴項**
   - 參見 `requirements.txt`
   - 安裝指令：`pip install -r requirements.txt`

## 安裝 Qt Creator

### Windows

1. 從 [Qt 官方網站](https://www.qt.io/download-qt-installer) 下載 Qt 線上安裝程式
2. 執行安裝程式
3. 在元件選擇中，至少選擇：
   - Qt Creator（在 Developer and Designer Tools 下）
4. 完成安裝

### macOS

```bash
# 使用 Homebrew 安裝
brew install --cask qt-creator

# 或從官方網站下載 .dmg 檔案
```

### Linux (Ubuntu/Debian)

```bash
# 從套件管理器安裝
sudo apt update
sudo apt install qtcreator

# 或使用 snap
sudo snap install qtcreator --classic
```

### Linux (Fedora/RHEL)

```bash
sudo dnf install qt-creator
```

## 在 Qt Creator 中開啟專案

### 方法 1：從 Qt Creator 開啟

1. 啟動 Qt Creator
2. 點擊選單：**檔案（File）** → **開啟檔案或專案（Open File or Project）...**
3. 導航到專案目錄
4. 選擇 `foot.pro` 檔案
5. 點擊 **開啟（Open）**

### 方法 2：從檔案管理器開啟

1. 在您的檔案管理器中導航到專案目錄
2. 雙擊 `foot.pro` 檔案
3. 系統應該會自動使用 Qt Creator 開啟

### 方法 3：從命令列開啟

```bash
# 導航到專案目錄
cd /path/to/foot

# 使用 Qt Creator 開啟專案
qtcreator foot.pro
```

## 專案配置

### 首次開啟時

首次在 Qt Creator 中開啟專案時，您可能會看到專案配置畫面：

1. **Configure Project（配置專案）**：點擊此按鈕接受預設設定
2. Qt Creator 將建立一個 `foot.pro.user` 檔案（此檔案已加入 `.gitignore`，不會提交到版本控制）

### 專案結構

開啟後，您將在左側的 **專案（Projects）** 面板中看到：

```
FootballAnalysis
├── Sources (原始檔案)
│   ├── qt_main.py              # Qt GUI 主程式
│   ├── main.py                 # 命令列主程式
│   ├── yolo_inference.py       # YOLO 推理
│   ├── camera_movement_estimator/
│   │   ├── __init__.py
│   │   └── camera_movement_estimator.py
│   ├── player_ball_assigner/
│   │   ├── __init__.py
│   │   └── player_ball_assigner.py
│   ├── speed_and_distance_estimator/
│   │   ├── __init__.py
│   │   └── speed_and_distance_estimator.py
│   ├── team_assigner/
│   │   ├── __init__.py
│   │   └── team_assigner.py
│   ├── trackers/
│   │   ├── __init__.py
│   │   └── tracker.py
│   ├── utils/
│   │   ├── __init__.py
│   │   ├── bbox_utils.py
│   │   └── video_utils.py
│   └── view_transformer/
│       ├── __init__.py
│       └── view_transformer.py
│
└── Other Files (其他檔案)
    ├── README.md               # 專案說明（中文）
    ├── README_EN.md            # 專案說明（英文）
    ├── README_ZH.md            # 專案說明（繁體中文）
    ├── USAGE_ZH.md             # 使用指南
    ├── QT_GUI_GUIDE.md         # Qt GUI 指南（英文）
    ├── QT_GUI_GUIDE_ZH.md      # Qt GUI 指南（中文）
    ├── 中文文檔.md              # 中文文檔
    ├── requirements.txt         # Python 依賴項
    ├── .gitignore              # Git 忽略檔案
    ├── run_qt_gui.sh           # Linux/Mac 執行腳本
    └── run_qt_gui.bat          # Windows 執行腳本
```

## 在 Qt Creator 中工作

### 瀏覽代碼

1. **檔案樹狀結構**
   - 使用左側的 **專案（Projects）** 面板瀏覽檔案
   - 雙擊檔案以在編輯器中開啟

2. **快速導航**
   - **Ctrl+K**（Windows/Linux）或 **Cmd+K**（macOS）：快速開啟檔案
   - **Ctrl+Shift+F**：在專案中搜尋文字
   - **F2**：跳轉到定義（對 Python 支援有限）

3. **分割視圖**
   - 右鍵點擊編輯器標籤
   - 選擇 **Split**（分割）以並排查看多個檔案

### 編輯代碼

1. **語法高亮顯示**
   - Qt Creator 自動識別 Python 檔案並提供語法高亮顯示

2. **縮排和格式化**
   - Qt Creator 遵循 Python 縮排規則
   - 使用 Tab 或空格（根據專案設定）

3. **註解**
   - **Ctrl+/**（Windows/Linux）或 **Cmd+/**（macOS）：切換行註解

### 執行專案

Qt Creator 主要用於代碼編輯和專案管理。要執行 Python 應用程式：

#### 選項 1：使用整合式終端機

1. 點擊選單：**工具（Tools）** → **外部（External）** → **文字（Text）** → **開啟終端機（Open Terminal）**
   - 或按 **Alt+T**，然後 **Alt+Shift+T**

2. 在終端機中執行：
   ```bash
   # 執行 Qt GUI 應用程式
   python qt_main.py
   
   # 或執行命令列版本
   python main.py
   ```

#### 選項 2：使用外部終端機

1. 開啟您的系統終端機（Terminal、命令提示字元、PowerShell）
2. 導航到專案目錄
3. 執行 Python 腳本

#### 選項 3：配置自訂執行配置

您可以在 Qt Creator 中配置自訂執行配置：

1. 點擊選單：**專案（Projects）** → **執行（Run）**
2. 點擊 **Add（新增）** → **Custom Executable（自訂可執行檔）**
3. 配置：
   - **Executable（可執行檔）**：`python` 或 `python3`
   - **Command line arguments（命令列參數）**：`qt_main.py`
   - **Working directory（工作目錄）**：`%{sourceDir}`
4. 點擊 **Apply（套用）**

現在您可以按 **Ctrl+R**（Windows/Linux）或 **Cmd+R**（macOS）來執行應用程式。

### 偵錯（有限支援）

Qt Creator 主要支援 C++ 偵錯，但您可以：

1. 使用 Python 偵錯器（pdb）在代碼中加入斷點：
   ```python
   import pdb; pdb.set_trace()
   ```

2. 或使用 Visual Studio Code 等專門的 Python IDE 進行偵錯

### 版本控制

Qt Creator 內建 Git 支援：

1. **查看更改**
   - 點擊選單：**工具（Tools）** → **Git** → **Diff（差異）**

2. **提交更改**
   - 點擊選單：**工具（Tools）** → **Git** → **Local Repository（本地儲存庫）** → **Commit（提交）**

3. **推送/拉取**
   - 使用 Git 選單中的相應選項

4. **查看歷史記錄**
   - 點擊選單：**工具（Tools）** → **Git** → **Log（日誌）**

## 有用的 Qt Creator 功能

### 1. 搜尋和替換

- **Ctrl+F**（Windows/Linux）或 **Cmd+F**（macOS）：在當前檔案中搜尋
- **Ctrl+Shift+F**：在整個專案中搜尋
- **Ctrl+H**（Windows/Linux）或 **Cmd+H**（macOS）：在當前檔案中替換

### 2. 書籤

- **Ctrl+M**：切換書籤
- **Ctrl+.**：跳轉到下一個書籤
- **Ctrl+,**：跳轉到上一個書籤

### 3. 程式碼片段

Qt Creator 支援自訂程式碼片段：

1. 點擊選單：**工具（Tools）** → **選項（Options）**
2. 選擇 **文字編輯器（Text Editor）** → **程式碼片段（Snippets）**
3. 新增 Python 程式碼片段

### 4. 外觀自訂

1. 點擊選單：**工具（Tools）** → **選項（Options）**
2. **環境（Environment）** → **介面（Interface）**：變更主題
3. **文字編輯器（Text Editor）** → **字型與顏色（Fonts & Colors）**：自訂編輯器外觀

## 常見問題解答

### 問：Qt Creator 可以執行 Python 代碼嗎？

答：Qt Creator 主要是一個代碼編輯器和專案管理工具。它可以透過整合式終端機或自訂執行配置來執行 Python 代碼，但不像 PyCharm 或 VS Code 那樣提供完整的 Python IDE 功能。

### 問：為什麼選擇 Qt Creator 而不是其他 Python IDE？

答：
- 如果您正在開發 PyQt5 應用程式，Qt Creator 提供了熟悉的環境
- 優秀的專案結構視覺化
- 輕量級且快速
- 跨平台一致性
- 對於已經熟悉 Qt Creator 的開發者來說很方便

### 問：我可以使用 Qt Designer 設計 GUI 嗎？

答：可以！雖然此專案使用程式碼方式建立 GUI（在 `qt_main.py` 中），但您可以：
1. 使用 Qt Designer 建立 `.ui` 檔案
2. 使用 `pyuic5` 轉換為 Python 代碼
3. 整合到專案中

```bash
# 將 .ui 檔案轉換為 Python
pyuic5 -x your_design.ui -o ui_your_design.py
```

### 問：foot.pro.user 檔案是什麼？

答：`foot.pro.user` 檔案儲存您的個人 Qt Creator 設定（視窗布局、執行配置等）。它已加入 `.gitignore`，不會被版本控制追蹤，因為這些設定是使用者特定的。

### 問：我可以使用 Qt Creator 的 C++ 功能嗎？

答：此專案是純 Python 專案，因此大多數 C++ 特定功能不適用。但是，Qt Creator 的編輯器、搜尋和專案管理功能完全適用於 Python 代碼。

### 問：如何更新 foot.pro 檔案？

答：`foot.pro` 檔案是一個文字檔，列出專案中的所有檔案。如果您新增或移除 Python 檔案或文檔，請編輯 `foot.pro` 並更新 `SOURCES` 或 `OTHER_FILES` 部分。

### 問：Qt Creator 能自動完成 Python 代碼嗎？

答：Qt Creator 提供基本的 Python 語法高亮顯示，但不提供像 PyCharm 或 VS Code（配合 Pylance）那樣的進階自動完成。對於進階 Python 開發功能，您可能需要考慮使用專門的 Python IDE。

### 問：我可以同時使用 Qt Creator 和其他 IDE 嗎？

答：當然可以！許多開發者使用：
- **Qt Creator**：用於專案概覽、檔案管理和快速編輯
- **PyCharm/VS Code**：用於深入的 Python 開發和偵錯
- **Jupyter Notebook**：用於實驗和原型開發

## 最佳實踐

### 1. 組織您的工作區

- 使用 Qt Creator 的專案視圖瀏覽檔案結構
- 使用書籤標記重要的代碼部分
- 利用分割視圖同時查看相關檔案

### 2. 利用整合式終端機

- 直接在 Qt Creator 內執行 Python 腳本
- 快速測試代碼更改
- 監控應用程式輸出

### 3. 使用版本控制

- 定期提交更改
- 編寫有意義的提交訊息
- 使用 Qt Creator 的 Git 整合查看差異

### 4. 保持 foot.pro 更新

- 新增新檔案時，更新 `foot.pro`
- 保持專案結構乾淨有組織
- 記錄重要的專案目錄

### 5. 自訂您的環境

- 調整編輯器字型和顏色以適應您的喜好
- 設定有用的鍵盤快捷鍵
- 配置自訂執行配置以提高效率

## 替代方案

雖然 Qt Creator 是一個很好的選擇，但以下是其他可以考慮的 IDE：

### PyCharm
- **優點**：完整的 Python IDE，優秀的偵錯，智慧自動完成
- **缺點**：比 Qt Creator 更重量級
- **適合**：專業 Python 開發

### Visual Studio Code
- **優點**：輕量級，優秀的 Python 擴充套件，大量客製化
- **缺點**：需要配置擴充套件
- **適合**：多語言開發者

### Spyder
- **優點**：類似 MATLAB 的介面，適合科學計算
- **缺點**：主要針對資料科學，不是通用 IDE
- **適合**：資料科學和分析

### Jupyter Notebook/Lab
- **優點**：互動式開發，適合實驗
- **缺點**：不適合完整應用程式開發
- **適合**：原型開發和探索性分析

## 額外資源

### 學習 Qt Creator

- [Qt Creator 官方文檔](https://doc.qt.io/qtcreator/)
- [Qt Creator 鍵盤快捷鍵](https://doc.qt.io/qtcreator/creator-keyboard-shortcuts.html)
- [Qt Creator 視訊教學](https://www.qt.io/qt-training/)

### 學習 PyQt5

- [PyQt5 官方文檔](https://www.riverbankcomputing.com/static/Docs/PyQt5/)
- [Qt for Python (PySide6) 文檔](https://doc.qt.io/qtforpython/)
- 參考此專案中的 `qt_main.py` 作為實際範例

### 專案特定文檔

- **[QT_GUI_GUIDE_ZH.md](QT_GUI_GUIDE_ZH.md)**：Qt GUI 應用程式使用指南
- **[README_ZH.md](README_ZH.md)**：專案概述和功能
- **[USAGE_ZH.md](USAGE_ZH.md)**：詳細使用說明

## 疑難排解

### Qt Creator 無法開啟 foot.pro

**解決方案**：
1. 確保已正確安裝 Qt Creator
2. 檢查 `foot.pro` 檔案是否存在且未損壞
3. 嘗試使用選單 **File → Open File or Project** 而不是雙擊

### 檔案未顯示在專案樹中

**解決方案**：
1. 檢查 `foot.pro` 檔案中是否列出了檔案
2. 在 `SOURCES` 或 `OTHER_FILES` 中新增缺少的檔案
3. 關閉並重新開啟專案以重新整理

### 無法從 Qt Creator 執行 Python

**解決方案**：
1. 確保 Python 已安裝並在系統 PATH 中
2. 使用整合式終端機而不是執行配置
3. 檢查 Python 可執行檔的路徑是否正確

### 中文字元顯示亂碼

**解決方案**：
1. 確保檔案以 UTF-8 編碼儲存
2. 在 Qt Creator 中設定：**Tools → Options → Text Editor → Behavior → File Encodings** 設為 UTF-8
3. 重新載入檔案

## 總結

使用 Qt Creator 開發此足球分析專案可以提供：

✅ 清晰的專案結構視覺化  
✅ 強大的代碼編輯和導航功能  
✅ 整合式終端機用於執行 Python  
✅ 內建版本控制支援  
✅ 跨平台開發環境  
✅ 適合 PyQt5 開發的熟悉工具  

雖然 Qt Creator 可能不提供像專門 Python IDE 那樣的所有進階功能，但它為此專案提供了一個優秀的輕量級開發環境，特別適合已經熟悉 Qt 生態系統的開發者。

---

**祝您使用 Qt Creator 開發愉快！** 🎯🖥️⚽

如有問題或需要協助，請參考專案中的其他文檔或在 GitHub 上提出 issue。
