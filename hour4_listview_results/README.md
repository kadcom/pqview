# Hour 4: ListView Results

## Objectives

- Add ListView control for query results display
- Use LVS_OWNERDATA for virtual list (efficient with large datasets)
- Dynamic column creation based on query results
- Display results in proper table format with gridlines
- Auto-size columns to fit content
- Keep debug output for logging
- Update resize handling for Results + Debug sections
- Handle NULL values in ListView
- Split window space: 60% results, 40% debug

## Files

- `CMakeLists.txt` - Same as Hour 3.1
- `main.c` - Enhanced with ListView control

## What's New from Hour 3.1

### ListView Control

**Virtual List with LVS_OWNERDATA:**
- Added ListView with `LVS_OWNERDATA` style for efficiency
- Can handle millions of rows without memory issues
- Data provided on-demand via `LVN_GETDISPINFO` notification
- Stores only one `PGresult` in memory (`g_currentResult`)

**Extended Styles:**
- `LVS_EX_FULLROWSELECT` - Entire row is selected
- `LVS_EX_GRIDLINES` - Show grid lines between cells
- `LVS_EX_DOUBLEBUFFER` - Prevents flicker during scroll

**Report Mode (LVS_REPORT):**
- Table view with columns and rows
- Column headers automatically displayed
- Vertical and horizontal scrollbars

### WM_NOTIFY Handler

**LVN_GETDISPINFO Notification:**
- Called when ListView needs to display an item
- Retrieves data from cached `g_currentResult`
- Handles NULL values properly
- Returns data in static buffer (lifetime requirement)

### Query Results Display

**Dynamic Columns:**
- Columns created based on query result structure
- Uses `PQfname()` to get column names
- Initial width: 120px
- Auto-sized to fit header text with `LVSCW_AUTOSIZE_USEHEADER`

**Virtual Items:**
- No items explicitly inserted
- Uses `ListView_SetItemCountEx()` to set count
- ListView requests data as needed for visible rows
- Extremely efficient for large result sets

### Layout Changes

**New Window Height:**
- Increased from 700px to 850px
- More space for results display

**Three Sections:**
1. Connection Settings (fixed 102px)
2. SQL Query (fixed 120px)
3. **Query Results (60% of remaining space, min 200px)** - NEW
4. Debug Output (40% of remaining space, min 150px)

**Resize Behavior:**
- Results and Debug split remaining vertical space
- Both expand horizontally with window
- Maintains 60/40 ratio on vertical resize

## Key Differences from Hour 3.1

| Aspect | Hour 3.1 | Hour 4 |
|--------|----------|--------|
| Results Display | Debug output (text) | ListView (table) |
| Data Storage | Temporary (formatted text) | Cached PGresult |
| Performance | Poor with large results | Excellent (virtual list) |
| Column Display | Fixed-width (20 chars) | Dynamic width (auto-size) |
| NULL Handling | In text output | In ListView cells |
| Row Limit | 100 rows display | Unlimited (virtual) |
| Scrolling | Debug listbox only | ListView + Debug listbox |
| Window Height | 700px | 850px |
| Layout | Query + Debug | Query + Results + Debug |

## Building

```bash
cd hour4_listview_results
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/17"
cmake --build . --config Release
Release\pqview.exe
```

## New Windows API Concepts

### LVS_OWNERDATA (Virtual List View)

**What is a virtual list?**
- ListView doesn't store item data itself
- Data provided on-demand by parent window
- Extremely memory-efficient
- Perfect for database result sets

**Creating virtual ListView:**
```c
CreateWindowEx(0, WC_LISTVIEW, "",
    WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA,
    ...);
```

**Setting item count:**
```c
ListView_SetItemCountEx(hwndListView, rowCount, LVSICF_NOINVALIDATEALL);
```

### WM_NOTIFY and LVN_GETDISPINFO

**WM_NOTIFY message:**
- Sent by child controls to parent window
- Contains notification code in NMHDR structure
- Used for complex control notifications

**Handling LVN_GETDISPINFO:**
```c
case WM_NOTIFY:
{
    LPNMHDR nmhdr = (LPNMHDR)lParam;
    if (nmhdr->code == LVN_GETDISPINFO)
    {
        NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
        int row = plvdi->item.iItem;
        int col = plvdi->item.iSubItem;

        /* Provide data for this cell */
        static char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s", GetDataForCell(row, col));
        plvdi->item.pszText = buffer;
    }
}
```

**Important:** Buffer must be static or global (not local variable)!

### ListView Columns (LVCOLUMN)

**Adding columns:**
```c
LVCOLUMN lvc;
ZeroMemory(&lvc, sizeof(LVCOLUMN));
lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
lvc.fmt = LVCFMT_LEFT;
lvc.cx = 120;  /* Width in pixels */
lvc.pszText = "Column Name";
ListView_InsertColumn(hwndListView, columnIndex, &lvc);
```

**Removing all columns:**
```c
while (ListView_DeleteColumn(hwndListView, 0))
    ;  /* Keep deleting column 0 until none left */
```

**Auto-sizing columns:**
```c
ListView_SetColumnWidth(hwndListView, columnIndex, LVSCW_AUTOSIZE_USEHEADER);
```

Options:
- `LVSCW_AUTOSIZE` - Size to fit items
- `LVSCW_AUTOSIZE_USEHEADER` - Size to fit header text (minimum)

### ListView Extended Styles

**Setting extended styles:**
```c
ListView_SetExtendedListViewStyle(hwndListView,
    LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
```

**Common extended styles:**
- `LVS_EX_FULLROWSELECT` - Click anywhere in row to select
- `LVS_EX_GRIDLINES` - Show grid between cells
- `LVS_EX_DOUBLEBUFFER` - Reduce flicker
- `LVS_EX_HEADERDRAGDROP` - Allow column reordering
- `LVS_EX_CHECKBOXES` - Add checkboxes to items

## Implementation Details

### Global Result Cache

**g_currentResult:**
- Stores current query result for virtual list
- Freed when new query executes
- Freed on window destruction
- **Do not call PQclear() after displaying results!**

**Memory Management:**
```c
/* Old result cleanup */
if (g_currentResult != NULL)
{
    PQclear(g_currentResult);
}

/* Store new result */
g_currentResult = res;  /* Don't call PQclear(res)! */
```

### Virtual List Data Flow

**Sequence:**
1. Execute query → Store result in `g_currentResult`
2. Set ListView item count with `ListView_SetItemCountEx()`
3. ListView displays → WM_NOTIFY with LVN_GETDISPINFO sent
4. WM_NOTIFY handler reads data from `g_currentResult`
5. Returns data to ListView for display

**Why virtual list?**
- Query returns 1 million rows → Only stores PGresult
- ListView visible area: ~20 rows → Only 20 LVN_GETDISPINFO calls
- Scroll down → New LVN_GETDISPINFO calls for new visible rows
- Memory usage: Minimal (PGresult + visible row buffers)

### Resize Calculation

**Space allocation:**
```c
availableHeight = clientHeight - connectionHeight - queryHeight - margins;
resultsGroupHeight = (int)(availableHeight * 0.6);  /* 60% */
debugGroupHeight = availableHeight - resultsGroupHeight - margin;  /* 40% */
```

**Minimums enforced:**
- Results: 200px minimum
- Debug: 150px minimum

## Common Issues

### ListView shows no data

**Cause:** Forgot to set item count or handle LVN_GETDISPINFO

**Solution:**
- Call `ListView_SetItemCountEx()` with row count
- Handle WM_NOTIFY with LVN_GETDISPINFO case

### ListView crashes on scroll

**Cause:** Using local buffer in LVN_GETDISPINFO

**Solution:** Use `static char buffer[]` (not local variable)

### Result set cleared too early

**Cause:** Called `PQclear(res)` before ListView finished displaying

**Solution:** Store result in `g_currentResult`, don't clear until next query

### Columns don't appear

**Cause:** Columns must be added before setting item count

**Solution:** Insert columns first, then call `ListView_SetItemCountEx()`

### NULL values crash

**Cause:** `PQgetvalue()` returns empty string for NULL

**Solution:** Check with `PQgetisnull()` first

## Testing Checklist

- [ ] Execute query with small result (10 rows) - should display in ListView
- [ ] Execute query with large result (1000+ rows) - should be fast
- [ ] Execute query with many columns (10+) - should show all columns
- [ ] Scroll through large result set - should be smooth
- [ ] Execute query with NULL values - should show "(NULL)"
- [ ] Execute new query - should replace previous results
- [ ] Resize window horizontally - ListView should expand
- [ ] Resize window vertically - Results/Debug should resize proportionally
- [ ] Select rows in ListView - should highlight entire row
- [ ] Column headers should be clickable (no sorting yet, but clickable)

## Virtual List Performance

**Test Case: 1,000,000 rows**

**Without LVS_OWNERDATA (normal ListView):**
- Insert time: ~30-60 seconds
- Memory usage: ~500MB-1GB
- UI freezes during insert
- Scroll lag

**With LVS_OWNERDATA (virtual):**
- Insert time: Instant (<1ms)
- Memory usage: ~PGresult size only
- UI responsive
- Smooth scrolling

**Why?**
- Normal ListView stores all items in memory
- Virtual ListView stores only item count
- Data retrieved on-demand for visible rows

## Stream Talking Points

1. **Why ListView?** - Proper table display with columns, not fixed-width text
2. **Why LVS_OWNERDATA?** - Can handle millions of rows efficiently
3. **Why cache PGresult?** - Virtual list needs data on-demand
4. **Why static buffer?** - ListView expects data to remain valid after return
5. **Why WM_NOTIFY?** - Child control notification mechanism
6. **Why not insert all items?** - Would be slow and memory-intensive
7. **Why 60/40 split?** - Results more important than debug output
8. **Why LVSCW_AUTOSIZE_USEHEADER?** - Ensures columns fit their names

## Next Steps (Hour 5)

- Add column sorting (click header to sort)
- Export results to CSV
- Copy selected cells to clipboard
- Better error handling UI
- Connection profiles/favorites
- Query history
- Polish and refinements

## Comparison

**Hour 3.1 (Resize Layout):**
- Query results shown as formatted text in debug output
- Fixed-width columns (20 chars)
- Limited to 100 rows display
- Poor performance with large results
- No proper table structure
- Debug output fills remaining space

**Hour 4 (ListView Results):**
- Query results in proper ListView table
- Dynamic columns based on query
- Unlimited rows (virtual list)
- Excellent performance (LVS_OWNERDATA)
- Proper table with gridlines
- Results (60%) + Debug (40%) split
- Professional appearance
- Can scroll through any size result set
