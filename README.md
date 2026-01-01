# Football Analysis with Qt UI

這個專案包含足球比賽分析功能，並提供Qt圖形介面來查看分析結果。

## 新功能：Qt UI 圖形介面

現在可以使用Qt圖形介面來查看分析結果，包括：
- 📹 影片播放與控制
- ⚽ 球隊控球率統計（Team 1 & Team 2）
- 🏃 總跑動距離（公里）
- ⏱️ 平均速度統計

## 快速開始

1. 安裝依賴：
```bash
cd foot-Function
pip install -r qt_requirements.txt
```

2. 生成測試數據（可選）：
```bash
python generate_test_data.py
```

3. 啟動Qt介面：
```bash
python qt_ui_app.py
```

或者運行分析並自動啟動介面：
```bash
python run_analysis_with_ui.py --run-analysis
```

詳細使用說明請參考 `foot-Function/QT_UI_README.md`
