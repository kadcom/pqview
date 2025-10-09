# Hour 2.1: Modern UI (Windows 95 Style)

## Objectives

- Improve visual appearance with Windows 95-style UI
- Add proper fonts (MS Shell Dlg for UI, Courier New for debug)
- Use Group Boxes to organize controls
- Add 3D borders and better spacing
- Implement proper tab order
- Make static text backgrounds transparent
- Fix debug output rendering with LISTBOX control

## Files

- `CMakeLists.txt` - Same as Hour 2
- `main.c` - Enhanced UI with Windows 95 styling and LISTBOX debug output

## What's New from Hour 2

### Visual Improvements

**Fonts:**
- **MS Shell Dlg 8pt** - Windows 95 standard dialog font for all UI controls
- **Courier New** - Monospace font for debug output (better readability)
- Applied to all controls for consistent appearance

**Group Boxes:**
- **Connection Settings** group box organizes connection parameters
- **Debug Output** group box contains debug window
- Provides visual separation and organization
- Connect button and Status moved OUTSIDE group box for cleaner layout

**Background Color:**
- Changed from `COLOR_WINDOW` (white) to `COLOR_BTNFACE` (light gray)
- Classic Windows 95 dialog appearance

**3D Effects:**
- `WS_EX_WINDOWEDGE` on main window for raised 3D border
- `WS_EX_CLIENTEDGE` on input controls for sunken 3D look
- Group boxes automatically have raised 3D appearance

**Better Spacing:**
- Increased control spacing for less cramped appearance
- Proper margins inside group boxes (10px)
- Aligned controls horizontally and vertically

**Tab Order:**
- Added `WS_TABSTOP` to all input controls
- Can press Tab to move between fields
- Logical tab order: Host → Port → User → Password → Database → Connect → Debug

**Transparent Labels:**
- Handle `WM_CTLCOLORSTATIC` to make labels transparent
- Labels blend with dialog background (no white boxes)

### Debug Output Fix

**Changed from EDIT to LISTBOX:**
- Hour 2 used multiline EDIT control - had text rendering/overlap issues
- Hour 2.1 uses LISTBOX control - each log line is a separate item
- No more text overdraw or scrolling glitches
- Cleaner rendering with monospace font

## Key Differences from Hour 2

| Aspect | Hour 2 | Hour 2.1 |
|--------|--------|----------|
| Background | White | Light gray (dialog color) |
| Font | System default | MS Shell Dlg 8pt |
| Debug Font | System default | Courier New (monospace) |
| Debug Control | EDIT (multiline) | LISTBOX |
| Organization | Flat layout | Group boxes |
| 3D Effects | Basic | Full 3D borders |
| Spacing | Tight | Comfortable |
| Tab Order | No | Yes |
| Label Background | White boxes | Transparent |
| Rendering Issues | Text overlap on scroll | None |

## Building

```bash
cd hour2.1_modern_ui
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/17"
cmake --build . --config Release
Release\pqview.exe
```

## New Windows API Concepts

### CreateFont()
Creates custom font with specific attributes:
```c
HFONT hFont = CreateFont(
    -11,                        /* Height (negative = character height) */
    0,                          /* Width (0 = default aspect ratio) */
    0,                          /* Escapement angle */
    0,                          /* Orientation angle */
    FW_NORMAL,                  /* Weight (FW_BOLD for bold) */
    FALSE,                      /* Italic */
    FALSE,                      /* Underline */
    FALSE,                      /* Strikeout */
    DEFAULT_CHARSET,            /* Character set */
    OUT_DEFAULT_PRECIS,         /* Output precision */
    CLIP_DEFAULT_PRECIS,        /* Clipping precision */
    DEFAULT_QUALITY,            /* Quality */
    DEFAULT_PITCH | FF_SWISS,   /* Pitch and family (or FIXED_PITCH | FF_MODERN) */
    "MS Shell Dlg"              /* Font name */
);
```

**Monospace Font:**
```c
g_hMonoFont = CreateFont(
    -13, 0, 0, 0, FW_NORMAL,
    FALSE, FALSE, FALSE,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_QUALITY,
    FIXED_PITCH | FF_MODERN,    /* Fixed pitch = monospace */
    "Courier New"
);
```

**Font Height:**
- Negative = character height (actual visible height)
- `-11` for 8pt UI font
- `-13` for 10pt monospace font (needs more space)

### LISTBOX Control

**Creating a LISTBOX:**
```c
hwndListBox = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "",
    WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
    LBS_NOINTEGRALHEIGHT | LBS_NOSEL,
    x, y, width, height, hwnd, NULL, NULL, NULL);
```

**LISTBOX Styles:**
- `LBS_NOINTEGRALHEIGHT` - Allows partial items (no blank space at bottom)
- `LBS_NOSEL` - Items cannot be selected (read-only behavior)
- `WS_VSCROLL` - Vertical scrollbar
- `WS_HSCROLL` - Horizontal scrollbar

**Adding Items:**
```c
int index = SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)"Text");
```

**Scrolling to Bottom:**
```c
SendMessage(hwndListBox, LB_SETTOPINDEX, index, 0);
```

**Why LISTBOX over EDIT for logs:**
- Each line is a separate item (no overlap)
- Better scrolling performance
- No text rendering bugs
- Cleaner with monospace fonts
- Can't accidentally modify text

### WM_SETFONT Message
Applies font to a control:
```c
SendMessage(hwndControl, WM_SETFONT, (WPARAM)hFont, TRUE);
```

### Group Box Control
Creates a labeled frame around controls:
```c
CreateWindowEx(0, "BUTTON", "Group Title",
    WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
    x, y, width, height, hwnd, NULL, NULL, NULL);
```

### WM_CTLCOLORSTATIC
Handles drawing of static controls:
```c
case WM_CTLCOLORSTATIC:
{
    HDC hdcStatic = (HDC)wParam;
    SetBkMode(hdcStatic, TRANSPARENT);
    return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
}
```

## Resource Cleanup

**Important:** Always delete GDI objects!
```c
case WM_DESTROY:
{
    if (g_hFont) DeleteObject(g_hFont);
    if (g_hBoldFont) DeleteObject(g_hBoldFont);
    if (g_hMonoFont) DeleteObject(g_hMonoFont);  /* Don't forget monospace! */
    // ... rest of cleanup
}
```

## UI Layout

```
┌───────────────────────────────────────────────────┐
│ ┌─ Connection Settings ─────────────────────────┐ │
│ │ Host: [localhost    ] Port: [5432]           │ │
│ │ User: [postgres     ] Password: [********]   │ │
│ │ Database: [stream_demo]                       │ │
│ └───────────────────────────────────────────────┘ │
│ [Connect]             Status: Connected          │
├───────────────────────────────────────────────────┤
│ ┌─ Debug Output ────────────────────────────────┐ │
│ │ [14:23:15] Application started                │ │
│ │ [14:23:20] Connecting to localhost:5432...    │ │
│ │ [14:23:21] Connected to database 'stream_d... │ │
│ │ [14:23:21] Server: PostgreSQL 17.1 on x86_... │ │
│ │                                                │ │
│ └───────────────────────────────────────────────┘ │
└───────────────────────────────────────────────────┘
```

## Common Issues

### Debug text is overlapping/garbled

This was the issue in Hour 2 with EDIT control. Hour 2.1 fixes it by using LISTBOX instead.

### Fonts look too small/large

Font height depends on DPI. `-11` is correct for 8pt at 96 DPI. For high-DPI displays, Windows automatically scales fonts.

### Labels have white backgrounds

Make sure you handle `WM_CTLCOLORSTATIC` and return the correct brush.

### Tab key doesn't work

Ensure controls have `WS_TABSTOP` flag and are created in the desired tab order.

## Debugging the Debug Output

**Hour 2 problem:**
- Used `"EDIT"` control with `ES_MULTILINE`
- Used `EM_REPLACESEL` to append text
- Text would render on top of itself when scrolling
- Font changes caused rendering glitches

**Hour 2.1 solution:**
- Use `"LISTBOX"` control instead
- Use `LB_ADDSTRING` to add each line as separate item
- Each item renders independently
- No overlap, clean scrolling

## Stream Talking Points

1. **Why MS Shell Dlg?** - Windows font mapping system, ensures correct system font
2. **Why COLOR_BTNFACE?** - Standard dialog color since Windows 95
3. **Why LISTBOX for logs?** - Each line is independent, no rendering bugs
4. **Why Courier New?** - Monospace font aligns columns of text perfectly
5. **Why move Connect outside group box?** - Cleaner visual hierarchy
6. **What is LBS_NOSEL?** - Makes listbox read-only (can't select items)

## Next Steps (Hour 3)

- Add query input text box (multi-line EDIT)
- Add Execute button
- Keep the Windows 95 styling
- Display query results in debug window
- Maybe add a second LISTBOX for query results

## Lessons Learned

**EDIT control issues:**
- Multiline EDIT with custom fonts can have rendering bugs
- Text appending with `EM_REPLACESEL` sometimes causes overlap
- Scrolling can corrupt the display
- Hard to debug, OS/font dependent

**LISTBOX benefits:**
- Each line is a discrete item
- Rendering is always clean
- Better for append-only logs
- Can programmatically access individual lines
- More reliable across Windows versions
