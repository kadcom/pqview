# Hour 3.1: Resize Layout

## Objectives

- Handle window resizing dynamically
- Prevent window from being resized below initial size (900x700)
- Resize all controls proportionally when window is resized
- Debug output group expands to fill available vertical space
- Controls expand horizontally to fill available width
- Maintain proper margins and spacing

## Files

- `CMakeLists.txt` - Same as Hour 3
- `main.c` - Enhanced with resize handling

## What's New from Hour 3

### Window Resize Handling

**WM_SIZE Message:**
- Handles window resize events
- Calls `ResizeControls()` to reposition all controls
- Updates control positions and sizes dynamically

**WM_GETMINMAXINFO Message:**
- Sets minimum window size to initial 900x700
- Prevents window from being made smaller than initial size
- Uses `MINMAXINFO` structure to set `ptMinTrackSize`

**ResizeControls() Function:**
- Called on every WM_SIZE message
- Gets client area dimensions with `GetClientRect()`
- Calculates new positions and sizes for all controls
- Uses `MoveWindow()` to resize and reposition controls
- Debug output group expands/shrinks to fill remaining vertical space

### Resize Behavior

**Horizontal Resizing:**
- All group boxes expand to fill window width
- Query input and debug output expand horizontally
- Connection controls stay fixed width (centered within group)
- Maintains 12px margins on left and right

**Vertical Resizing:**
- Connection Settings group: Fixed 102px height
- Connect button/Status: Fixed 24px height
- SQL Query group: Fixed 120px height
- Debug Output group: **Expands to fill remaining space**
- Minimum debug group height: 150px

**Layout Math:**
```
debugHeight = clientHeight - margin - connectionHeight - margin
              - connectButtonHeight - margin - queryHeight - margin
```

## Key Differences from Hour 3

| Aspect | Hour 3 | Hour 3.1 |
|--------|--------|----------|
| Window Resizable | Yes (but controls don't resize) | Yes (controls resize) |
| Minimum Window Size | None (can resize to tiny) | 900x700 (enforced) |
| Control Positioning | Fixed at creation | Dynamic (MoveWindow) |
| Debug Output Height | Fixed 350px | Expands to fill space |
| Group Box Width | Fixed 860px | Expands with window |
| Resize Handler | None | WM_SIZE + ResizeControls() |
| Stored Group Handles | No | Yes (g_hwndConnectionGroup, etc.) |

## Building

```bash
cd hour3.1_resize_layout
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/17"
cmake --build . --config Release
Release\pqview.exe
```

## New Windows API Concepts

### WM_SIZE Message

**Message sent when window is resized:**
```c
case WM_SIZE:
{
    ResizeControls(hwnd);
    return 0;
}
```

**wParam values:**
- `SIZE_RESTORED` - Window restored to normal size
- `SIZE_MINIMIZED` - Window minimized
- `SIZE_MAXIMIZED` - Window maximized
- `SIZE_MAXSHOW` / `SIZE_MAXHIDE` - Other window affected

**lParam:**
- LOWORD(lParam) = new width
- HIWORD(lParam) = new height

### WM_GETMINMAXINFO Message

**Sets minimum/maximum tracking size:**
```c
case WM_GETMINMAXINFO:
{
    MINMAXINFO* mmi = (MINMAXINFO*)lParam;
    mmi->ptMinTrackSize.x = WINDOW_WIDTH;  /* 900 */
    mmi->ptMinTrackSize.y = WINDOW_HEIGHT; /* 700 */
    return 0;
}
```

**MINMAXINFO structure:**
```c
typedef struct tagMINMAXINFO {
    POINT ptReserved;
    POINT ptMaxSize;        /* Maximum size when maximized */
    POINT ptMaxPosition;    /* Position when maximized */
    POINT ptMinTrackSize;   /* Minimum tracking size */
    POINT ptMaxTrackSize;   /* Maximum tracking size */
} MINMAXINFO;
```

**Important:** This message is sent BEFORE window is created, so it applies to initial sizing too.

### GetClientRect()

**Gets client area dimensions:**
```c
RECT rcClient;
GetClientRect(hwnd, &rcClient);
int clientWidth = rcClient.right - rcClient.left;
int clientHeight = rcClient.bottom - rcClient.top;
```

**Client area vs Window area:**
- Client area: Inside the borders and title bar
- Window area: Includes borders, title bar, menu

### MoveWindow()

**Moves and resizes a control:**
```c
MoveWindow(hwndControl, x, y, width, height, bRepaint);
```

**Parameters:**
- `hwndControl` - Handle to control to resize
- `x, y` - New position (relative to parent)
- `width, height` - New size
- `bRepaint` - TRUE to repaint, FALSE to not repaint

**bRepaint = TRUE:**
- Window redraws immediately
- Use when resizing individual controls
- Prevents flicker by redrawing once

## Resize Implementation Details

### Global Handles Added

**Group box handles stored for resizing:**
```c
HWND g_hwndConnectionGroup, g_hwndQueryGroup, g_hwndDebugGroup;
```

**Why store these?**
- Need to resize group boxes dynamically
- MoveWindow() requires HWND handle
- Previously used temporary `hwndControl` variable

### Layout Calculations

**Fixed heights:**
- Connection group: 102px
- Connect button: 24px
- Query group: 120px
- Margins: 12px between sections

**Variable height:**
- Debug group: Fills remaining vertical space
- Minimum: 150px (prevents tiny debug window)

**Margins:**
- Window margin: 12px
- Group box internal padding: 10px
- Spacing between sections: 10px

**Width calculations:**
```c
groupWidth = clientWidth - (2 * margin);  /* Full width minus margins */
controlWidth = groupWidth - 20;            /* Inside group, minus padding */
```

### Control Positioning

**Connection group controls:**
- Group box: Full width
- Internal controls: Fixed positions within group
- Host, Port, User, Password, Database: Stay at fixed positions

**Query group controls:**
- Group box: Full width
- Query input: Expands horizontally (groupWidth - 20)
- Execute button: Fixed size (120x24)

**Debug group controls:**
- Group box: Full width, variable height
- Debug listbox: Expands both horizontally and vertically
- Height calculation: `debugGroupHeight - 35` (leaves room for group label)

## Testing Checklist

- [ ] Resize window horizontally - controls should expand width
- [ ] Resize window vertically - debug output should expand height
- [ ] Try to make window smaller than 900x700 - should be blocked
- [ ] Maximize window - should fill screen properly
- [ ] Restore from maximized - should restore to previous size
- [ ] Type long query - should wrap properly in resized input
- [ ] Execute query with results - should display in resized debug window
- [ ] Resize while connected - connection should remain active
- [ ] Minimize and restore - should work correctly

## Common Issues

### Controls don't resize

**Cause:** Forgot to handle WM_SIZE message

**Solution:** Add WM_SIZE case that calls ResizeControls()

### Can resize window too small

**Cause:** Not handling WM_GETMINMAXINFO

**Solution:** Set ptMinTrackSize in WM_GETMINMAXINFO handler

### Controls flicker during resize

**Cause:** Multiple repaints during resize

**Solution:** Use bRepaint = TRUE in MoveWindow() (already implemented)

### Debug output too small when resized

**Cause:** debugGroupHeight calculation results in negative/small value

**Solution:** Add minimum height check (150px minimum)

### Group boxes don't resize

**Cause:** Forgot to store group box handles

**Solution:** Store g_hwndConnectionGroup, g_hwndQueryGroup, g_hwndDebugGroup

## Layout Formula Summary

```
Y positions (vertical):
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
margin (12)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Connection Group (102)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
margin (10)
Connect Button (24)
margin (10)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Query Group (120)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
margin (10)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Debug Group (VARIABLE - fills rest)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
margin (12)

Total fixed vertical: 12+102+10+24+10+120+10+12 = 300px
Debug height = clientHeight - 300

X positions (horizontal):
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
margin (12)  |  GROUP BOX (clientWidth - 24)  | margin (12)
             |  [10px padding on each side]   |
             |  Controls inside group         |
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## Stream Talking Points

1. **Why resize controls?** - Professional UI behavior, users expect it
2. **Why minimum size?** - Prevents unusable tiny window, protects layout
3. **Why WM_GETMINMAXINFO?** - Prevents resize below minimum during drag
4. **Why store group handles?** - Need them for MoveWindow() calls
5. **Why debug expands vertically?** - Most useful space for query results
6. **Why GetClientRect?** - Need current size to calculate new positions
7. **Why bRepaint = TRUE?** - Prevents flicker by doing single repaint

## Next Steps (Hour 4)

- Add ListView control for query results
- Replace formatted text results with proper table
- Dynamic columns based on query
- Keep debug output for logging
- Resize ListView when window resizes

## Comparison

**Hour 3 (Query Execution):**
- Fixed layout at creation
- Can resize window but controls don't adjust
- No minimum window size
- Debug output fixed at 350px height

**Hour 3.1 (Resize Layout):**
- Dynamic layout on resize
- All controls resize proportionally
- Minimum window size: 900x700
- Debug output expands to fill available space
- Maintains proper margins and spacing
- Professional resize behavior
