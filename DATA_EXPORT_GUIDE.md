# Data Export Feature Guide

## Overview

The Football Analysis System now includes a comprehensive data export feature that allows you to extract all analysis results to structured file formats (JSON and CSV). This enables further analysis, integration with other tools, and archival of game statistics.

## What Gets Exported?

The system exports five different types of data files:

### 1. Player Statistics CSV (`*_player_stats.csv`)
Contains frame-by-frame player data:
- Player ID and team assignment
- Bounding box coordinates (x1, y1, x2, y2)
- Position coordinates (x, y)
- Speed in km/h
- Cumulative distance in meters
- Ball possession indicator (True/False)

**Use Cases:**
- Create player performance charts
- Analyze movement patterns
- Calculate average speeds over time periods
- Track individual player contributions

### 2. Ball Tracking JSON (`*_ball_tracking.json`)
Contains frame-by-frame ball data:
- Ball position per frame
- Bounding box coordinates

**Use Cases:**
- Analyze ball movement patterns
- Track ball trajectory
- Calculate ball speed and distance

### 3. Team Possession JSON (`*_team_possession.json`)
Contains team possession statistics:
- Frames with possession for each team
- Possession percentages
- Frame-by-frame possession record

**Use Cases:**
- Compare team dominance
- Analyze possession trends over time
- Identify possession streaks

### 4. Camera Movement CSV (`*_camera_movement.csv`)
Contains frame-by-frame camera movement:
- X-axis movement in pixels
- Y-axis movement in pixels

**Use Cases:**
- Analyze camera operator behavior
- Compensate for camera movement in custom analysis
- Track focus areas during the game

### 5. Comprehensive Summary JSON (`*_summary.json`)
Contains aggregated statistics:
- Player summary (max speed, total distance, ball possession time)
- Team possession summary
- Ball detection statistics
- Camera movement totals
- Analysis metadata (timestamp, total frames)

**Use Cases:**
- Quick overview of game statistics
- Generate reports
- Compare multiple games

## How to Use

### Option 1: GUI Application (Easiest)

1. Launch the Qt GUI:
```bash
python qt_main.py
```

2. Select your input video
3. Enable "Export analysis data (JSON/CSV)" checkbox (enabled by default)
4. Click "Start Analysis"
5. Find exported files in the output directory

### Option 2: Command Line (Most Flexible)

```bash
# Basic usage (exports to output_videos/)
python main.py --input my_video.mp4

# Custom export directory
python main.py --input my_video.mp4 --export-dir results/game1

# Custom filename prefix
python main.py --input my_video.mp4 --export-prefix match_2024_01_15

# Disable export
python main.py --input my_video.mp4 --no-export

# Full customization
python main.py \
  --input videos/game.mp4 \
  --output results/annotated.avi \
  --export-dir results/data \
  --export-prefix game_analysis
```

## Analyzing Exported Data

### Using the Provided Example Script

```bash
# Analyze data in default location
python analyze_exported_data.py

# Analyze data in custom location
python analyze_exported_data.py results/data
```

This script demonstrates:
- Loading and parsing all exported files
- Calculating additional statistics
- Finding possession streaks
- Displaying comprehensive game analysis

### Using Python (Pandas)

```python
import pandas as pd
import json

# Load player statistics
df = pd.read_csv('output_videos/analysis_player_stats_TIMESTAMP.csv')

# Filter data for a specific player
player_1 = df[df['player_id'] == 1]

# Calculate average speed
avg_speed = player_1['speed_kmh'].mean()
print(f"Player 1 average speed: {avg_speed:.2f} km/h")

# Plot player distance over time
import matplotlib.pyplot as plt
plt.plot(player_1['frame'], player_1['distance_m'])
plt.xlabel('Frame')
plt.ylabel('Distance (m)')
plt.title('Player 1 Distance Over Time')
plt.show()
```

### Using Python (JSON)

```python
import json

# Load comprehensive summary
with open('output_videos/analysis_summary_TIMESTAMP.json', 'r') as f:
    summary = json.load(f)

# Access team possession
team1_pct = summary['team_possession']['team_1']['percentage']
team2_pct = summary['team_possession']['team_2']['percentage']

print(f"Team 1: {team1_pct}% possession")
print(f"Team 2: {team2_pct}% possession")

# Access player stats
for player_id, stats in summary['player_summary'].items():
    print(f"Player {player_id}: {stats['max_speed_kmh']:.1f} km/h max speed")
```

### Using Excel/Google Sheets

1. Open the CSV files directly in Excel or Google Sheets
2. Create pivot tables to summarize data
3. Generate charts and graphs
4. Filter and sort by different columns

### Using Data Analysis Tools

The exported formats are compatible with:
- **R**: Use `read.csv()` and `jsonlite::fromJSON()`
- **MATLAB**: Use `readtable()` and `jsondecode()`
- **Tableau**: Import CSV files directly
- **Power BI**: Import CSV files directly
- **SQL databases**: Load CSV files into database tables

## File Naming Convention

All exported files follow this naming pattern:
```
{prefix}_{type}_{timestamp}.{extension}

Examples:
- analysis_player_stats_20240115_143022.csv
- match1_summary_20240115_143022.json
- game_team_possession_20240115_143022.json
```

Where:
- `prefix`: Customizable prefix (default: "analysis")
- `type`: File type (player_stats, ball_tracking, etc.)
- `timestamp`: Date and time of export (YYYYMMDD_HHMMSS)
- `extension`: File format (csv or json)

## Tips and Best Practices

### Storage
- Exported files are typically 10-50 KB for a 5-minute video
- Store files in organized directories by game/date
- Keep summary files for quick reference

### Analysis
- Use the summary file for quick insights
- Use detailed CSV files for in-depth analysis
- Combine multiple game exports to track trends over time

### Automation
- Use command-line arguments in scripts
- Batch process multiple videos
- Automatically archive exports to cloud storage

### Performance
- Export adds minimal processing time (< 1 second)
- No impact on video processing speed
- Files are written after video rendering completes

## Troubleshooting

### Files Not Created
- Check that export is enabled (not using `--no-export`)
- Verify output directory has write permissions
- Check disk space availability

### Cannot Open Files
- Ensure JSON files are valid using online validators
- Open CSV files with UTF-8 encoding
- Some editors may require explicit encoding selection

### Missing Data
- Some fields may be empty if data is unavailable (e.g., no ball possession)
- Check the original video analysis for issues
- Verify all analysis steps completed successfully

## Advanced Usage

### Custom Post-Processing

Create custom analysis scripts that:
- Compare player performance across multiple games
- Generate automated reports
- Calculate custom metrics
- Create visualizations and heatmaps

### Integration with Other Tools

- Import into sports analysis software
- Feed data into machine learning models
- Create real-time dashboards
- Build player performance databases

## Examples

See the `analyze_exported_data.py` script for a complete example that:
- Loads all exported files
- Calculates additional statistics
- Displays formatted output
- Demonstrates common analysis patterns

## Support

For issues or questions:
1. Check this guide for common solutions
2. Review the example script for usage patterns
3. Ensure all files were exported successfully
4. Check file permissions and disk space

---

**Happy Analyzing!** 📊⚽
