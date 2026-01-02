# Visual Hierarchy Implementation

## User Request (Comment #3705929447)
Create clear information hierarchy in the UI:
- Font size hierarchy: Title > GroupBox titles > Field labels
- Add required field markers (* or (required)) without changing validation logic
- Consistent visual rhythm: Block spacing 20px, Field spacing 10-12px

---

## Implementation Summary

### 1. Font Size Hierarchy

**Hierarchy Levels:**
```
Window Title (System default, ~12-13pt)
    ↓
GroupBox Titles (11pt, bold)
    ↓
Field Labels (10pt, normal)
    ↓
Content Text (9pt, normal)
```

**Implementation:**

**GroupBox Titles (11pt, bold):**
```cpp
inputGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
controlGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
summaryGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
detailsGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
actionsGroup->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
resultsGroupBox->setStyleSheet("QGroupBox { font-size: 11pt; font-weight: bold; }");
```

**Field Labels (10pt):**
```cpp
QLabel *videoLabel = new QLabel("Video File: <span style='color: red;'>*</span>", this);
videoLabel->setStyleSheet("font-size: 10pt;");

QLabel *modelLabel = new QLabel("YOLO Model: <span style='color: red;'>*</span>", this);
modelLabel->setStyleSheet("font-size: 10pt;");
```

**Content Text (9pt):**
```cpp
outputTextEdit->setStyleSheet("font-size: 9pt;");
statusLabel->setStyleSheet("... font-size: 10pt; ...");  // Slightly larger for visibility
```

### 2. Required Field Markers

**Implementation:**
Both input fields marked as required with red asterisk (*):

```cpp
// Video File - Required
QLabel *videoLabel = new QLabel("Video File: <span style='color: red;'>*</span>", this);

// YOLO Model - Required
QLabel *modelLabel = new QLabel("YOLO Model: <span style='color: red;'>*</span>", this);
```

**Visual Appearance:**
- Red asterisk (*) after label text
- Color: red (high visibility)
- No validation logic added (as requested)
- Users can see which fields are required at a glance

**Rationale:**
- Both fields are essential for analysis to work
- Video File: Required input for processing
- YOLO Model: Required model for detection
- Red color convention for required fields is universally understood

### 3. Consistent Visual Rhythm

**Spacing System:**

**Block Spacing (20px):**
```cpp
// Main grid spacing between major blocks
mainGrid->setSpacing(20);  // Was 12px, now 20px for better visual separation
```

This creates clear visual separation between:
- Input Configuration / Analysis Control (Row 0)
- Log Container (Row 2)
- Analysis Results (Row 4)

**Field Spacing (10px):**
```cpp
// Within forms and controls
inputFormLayout->setSpacing(10);   // Was 12px, now 10px for consistency
controlLayout->setSpacing(10);     // Was 12px, now 10px for consistency
logContainerLayout->setSpacing(10); // Was 12px, now 10px for consistency
```

**Internal Spacing (8px):**
```cpp
// Small internal elements
videoRowLayout->setSpacing(8);     // Between input and button
modelRowLayout->setSpacing(8);
actionsButtonLayout->setSpacing(8);
summaryGroupLayout->setSpacing(8);
actionsGroupLayout->setSpacing(8);
```

**Spacing Hierarchy:**
```
Block Spacing:    20px  (Between major sections)
    ↓
Field Spacing:    10px  (Between fields within a section)
    ↓
Internal Spacing:  8px  (Between elements within a field)
```

---

## Layout Structure Summary

### Main Grid Layout (QGridLayout)
```
Row 0, Col 0: Input Configuration GroupBox
              └─ QFormLayout (spacing: 10px)
                 ├─ Video File: [*] (font: 10pt)
                 └─ YOLO Model: [*] (font: 10pt)

Row 0, Col 1: Analysis Control GroupBox
              └─ QVBoxLayout (spacing: 10px)
                 └─ Start Analysis Button

[20px block spacing via mainGrid->setSpacing(20)]

Row 2, Col 0-1: Log Container (colspan=2)
                └─ QVBoxLayout (spacing: 10px)
                   ├─ Summary GroupBox
                   │  └─ Status Label (font: 10pt)
                   ├─ Details GroupBox
                   │  └─ QTextEdit (font: 9pt)
                   └─ Actions GroupBox
                      └─ Clear Log Button

[20px block spacing via mainGrid->setSpacing(20)]

Row 4, Col 0-1: Analysis Results GroupBox (colspan=2)
                └─ QVBoxLayout
                   └─ QTabWidget
                      ├─ Summary Tab
                      ├─ Data Table Tab
                      └─ Video Output Tab

Row 5: Stretch spacer (pushes content to top)
```

### All GroupBox Titles
- Font: 11pt, bold
- Components: Input Configuration, Analysis Control, Summary, Details, Actions, Analysis Results

### All Field Labels
- Font: 10pt, normal
- Components: Video File*, YOLO Model*
- Required markers: Red asterisk (*)

### Content Text
- Font: 9pt, normal
- Components: outputTextEdit (log content)
- Exception: statusLabel uses 10pt for better visibility

---

## Font Size Rationale

**11pt for GroupBox Titles:**
- Clearly larger than field labels (10pt)
- Bold weight adds emphasis
- Not too large to dominate the interface
- Maintains professional appearance

**10pt for Field Labels:**
- Standard readable size for labels
- Smaller than GroupBox titles (11pt)
- Larger than content text (9pt)
- Clear hierarchy established

**9pt for Content Text:**
- Suitable for reading detailed logs
- Doesn't overwhelm with size
- Allows more content to be visible
- Exception: Status label uses 10pt as it's important quick-view information

**System Default for Window Title:**
- Qt's default title bar font (~12-13pt typically)
- Automatically larger than all internal elements
- Maintains OS consistency

---

## Spacing Rationale

**20px Block Spacing:**
- Clear visual separation between major functional areas
- Breathing room between unrelated content
- Prevents interface from feeling cramped
- Follows common UI design patterns

**10px Field Spacing:**
- Comfortable spacing between related fields
- Not too tight, not too loose
- Maintains visual grouping within sections
- Consistent across all forms

**8px Internal Spacing:**
- Tight spacing for closely related elements
- E.g., input field and its Browse button
- Maintains visual unity of compound widgets
- Prevents elements from feeling disconnected

---

## Visual Rhythm Impact

### Before (Inconsistent)
```
Block spacing: 12px (too small)
Field spacing: 12px (same as blocks)
Internal: 8px
→ Poor visual separation between blocks and fields
```

### After (Consistent)
```
Block spacing: 20px (clear separation)
Field spacing: 10px (comfortable, grouped)
Internal: 8px (tight, unified)
→ Clear visual hierarchy: Blocks > Fields > Internal elements
```

---

## Required Field Markers

### Visual Convention
- **Red asterisk (*)**: Universal symbol for required fields
- Color: `color: red;` in HTML span
- Placement: Immediately after label text (e.g., "Video File: *")

### User Benefits
1. **Immediate Clarity**: Users see which fields are mandatory
2. **No Functional Change**: Validation logic untouched (as requested)
3. **Standard Convention**: Red asterisk is universally understood
4. **High Visibility**: Red color draws attention appropriately

### Implementation Note
Using HTML in QLabel allows styled text without custom painting:
```cpp
QLabel *label = new QLabel("Field Name: <span style='color: red;'>*</span>", this);
```

This is simpler than:
- Creating separate QLabel for asterisk
- Using rich text format
- Custom painting

---

## Complete Font & Spacing Reference

### Font Sizes
| Element | Size | Weight | Usage |
|---------|------|--------|-------|
| Window Title | System (~12-13pt) | Bold | Application title |
| GroupBox Titles | 11pt | Bold | Section headers |
| Field Labels | 10pt | Normal | Input labels |
| Status Label | 10pt | Normal | Quick status view |
| Content Text | 9pt | Normal | Detailed logs |

### Spacing Values
| Level | Size | Usage |
|-------|------|-------|
| Block | 20px | Between major sections (mainGrid) |
| Field | 10px | Between fields in forms |
| Internal | 8px | Between elements in a field |

### Margins
| Context | Value |
|---------|-------|
| Main grid | 16px |
| GroupBox interiors | 16px |
| Tab content | 5px |

---

## Benefits

✅ **Clear Visual Hierarchy** - 3-level font system (11pt > 10pt > 9pt)  
✅ **Required Field Clarity** - Red asterisks mark required inputs  
✅ **Consistent Rhythm** - 20/10/8 spacing system throughout  
✅ **Professional Appearance** - Bold GroupBox titles stand out  
✅ **Better Readability** - Appropriate font sizes for each context  
✅ **No Functional Changes** - Only visual/layout improvements  
✅ **Maintained Spacing** - Clear separation between blocks and fields  

---

## Code Changes Summary

**Files Modified:**
- MainWindow.cpp (setupUI function)

**Changes:**
1. Added font-size styling to all GroupBox titles (6 GroupBoxes)
2. Created explicit QLabel objects for required field markers (2 labels)
3. Updated mainGrid spacing from 12px to 20px (block spacing)
4. Updated field spacing from 12px to 10px (consistent field spacing)
5. Updated logContainerLayout spacing from 12px to 10px
6. Added font-size styling to outputTextEdit (9pt) and statusLabel (10pt)
7. Removed redundant setRowMinimumHeight calls (using mainGrid spacing instead)

**Lines Changed:** ~30 lines modified/added

**No Functional Changes:**
- All existing functionality preserved
- No validation logic added
- Visual/layout improvements only
- Signal/slot connections unchanged

---

## Testing Notes

### Visual Verification
1. **Font Hierarchy**: GroupBox titles should be noticeably larger and bolder than field labels
2. **Required Markers**: Red asterisks should be visible after "Video File:" and "YOLO Model:"
3. **Spacing**: Clear visual separation between Input/Control and Log sections (20px)
4. **Field Spacing**: Comfortable spacing between Video File and YOLO Model fields (10px)
5. **Log Text**: Smaller text in Details section for more content visibility (9pt)

### Consistency Check
- All GroupBox titles: 11pt, bold
- All field labels: 10pt, normal
- Block spacing: 20px throughout
- Field spacing: 10px throughout
- Internal spacing: 8px throughout
