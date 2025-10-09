# Hour 5: Polish & Features

This iteration adds polish and quality-of-life features to the PostgreSQL query client.

## New Features

### 1. Keyboard Shortcuts (Accelerators)
- **F5** - Execute current query
- **Ctrl+L** - Clear results
- **Ctrl+C** - Copy selected row to clipboard

Implemented using Windows accelerator tables (`CreateAcceleratorTable` and `TranslateAccelerator`).

### 2. Clear Results Button
Clears the ListView and cached query results without disconnecting from the database.

### 3. Copy Selected Row
Copies the currently selected row to the clipboard in tab-separated format (TSV).
- Select any row in the ListView
- Click "Copy Selected" or press Ctrl+C
- Paste into Excel, Notepad, or any text editor

### 4. Row Count Display
Shows the number of rows returned by the last query execution.
- Updates after each query execution
- Clears when results are cleared or disconnected

## Technical Implementation

### Accelerator Table
```c
ACCEL accel[3];
accel[0].fVirt = FVIRTKEY;           /* F5 */
accel[0].key = VK_F5;
accel[0].cmd = IDM_EXECUTE_QUERY;

accel[1].fVirt = FVIRTKEY | FCONTROL; /* Ctrl+L */
accel[1].key = 'L';
accel[1].cmd = IDM_CLEAR_RESULTS;

accel[2].fVirt = FVIRTKEY | FCONTROL; /* Ctrl+C */
accel[2].key = 'C';
accel[2].cmd = IDM_COPY_RESULTS;

g_hAccel = CreateAcceleratorTable(accel, 3);
```

### Message Loop Update
```c
while (GetMessage(&msg, NULL, 0, 0))
{
    if (!TranslateAccelerator(hwnd, g_hAccel, &msg))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
```

### Clipboard Copy Implementation
Uses Windows Clipboard API:
1. `OpenClipboard(hwnd)` - Open clipboard access
2. `EmptyClipboard()` - Clear existing data
3. `GlobalAlloc(GMEM_MOVEABLE, size)` - Allocate global memory
4. `GlobalLock(hMem)` - Lock memory for writing
5. `SetClipboardData(CF_TEXT, hMem)` - Set clipboard data
6. `CloseClipboard()` - Release clipboard

### Row Count Display
Simple static text control that updates via `SetWindowText`:
```c
void UpdateRowCount(int rowCount)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Rows: %d", rowCount);
    SetWindowText(g_hwndRowCount, buffer);
}
```

## UI Layout Changes

The ListView height was reduced to accommodate the new buttons and row count label below it:

```
┌─────────────────────────────────────────┐
│ Query Results                           │
│ ┌─────────────────────────────────────┐ │
│ │ [ListView - Virtual mode]           │ │
│ │                                     │ │
│ │                                     │ │
│ └─────────────────────────────────────┘ │
│ [Clear Results] [Copy Selected] Rows: 5 │
└─────────────────────────────────────────┘
```

## Building

Same as previous hours:
```bash
cd hour5_polish_features
mkdir build
cd build
cmake ..
cmake --build .
```

## Testing Keyboard Shortcuts

1. Connect to database
2. Enter a query
3. Press **F5** to execute (instead of clicking Execute)
4. Press **Ctrl+L** to clear results
5. Execute another query
6. Select a row and press **Ctrl+C**
7. Paste into Notepad to verify clipboard data

## What's New Since Hour 4.1

| Feature | Hour 4.1 | Hour 5 |
|---------|----------|--------|
| Execute shortcut | ❌ | ✅ F5 |
| Clear results | ❌ | ✅ Button + Ctrl+L |
| Copy row | ❌ | ✅ Button + Ctrl+C |
| Row count | ❌ | ✅ Display label |
| Accelerators | ❌ | ✅ Full support |

## Code Statistics

- **Lines of code:** ~920
- **Functions:** 15
- **New control IDs:** 3 buttons + 3 menu commands
- **Windows APIs used:** CreateAcceleratorTable, TranslateAccelerator, OpenClipboard, SetClipboardData
