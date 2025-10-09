# Hour 3: Query Execution

## Objectives

- Add SQL query input control (multi-line EDIT)
- Implement Execute Query button
- Execute SELECT queries using libpq
- Display results in debug output window
- Show column headers and formatted rows
- Add execution time measurement
- Handle NULL values
- Error handling for invalid queries
- Limit result display to prevent overflow

## Files

- `CMakeLists.txt` - Same as Hour 2.2
- `main.c` - Enhanced with query execution functionality

## What's New from Hour 2.2

### SQL Query Input

**Multi-line EDIT Control:**
- Added query input text box with `ES_MULTILINE`
- Default query: `SELECT * FROM customers LIMIT 10;`
- Uses monospace font (Consolas) for better readability
- `ES_WANTRETURN` allows Enter key to create new lines
- `ES_AUTOVSCROLL` with `WS_VSCROLL` for scrolling

**Execute Button:**
- Placed below query input
- Triggers query execution when clicked
- Validates connection before executing

### Query Execution

**HandleExecuteQuery() Function:**
```c
void HandleExecuteQuery(HWND hwnd)
{
    /* 1. Check database connection */
    if (g_conn == NULL || PQstatus(g_conn) != CONNECTION_OK) { ... }

    /* 2. Get query text from control */
    GetWindowText(g_hwndQuery, query, sizeof(query));

    /* 3. Execute query with timing */
    startTime = GetTickCount();
    res = PQexec(g_conn, query);
    endTime = GetTickCount();

    /* 4. Check result status */
    if (PQresultStatus(res) != PGRES_TUPLES_OK && ...) { ... }

    /* 5. Display results */
    nRows = PQntuples(res);
    nCols = PQnfields(res);

    /* 6. Format and display column headers */
    /* 7. Format and display data rows */
    /* 8. Clean up with PQclear(res) */
}
```

### Result Display

**Column Headers:**
- Displays all column names from query result
- Each column formatted to 20 characters width
- Separator line for readability
```
customer_id          first_name           last_name            email
-------------------- -------------------- -------------------- --------------------
```

**Data Rows:**
- Fixed-width columns (20 characters each)
- Long values truncated with "..." suffix
- NULL values displayed as "(NULL)"
- Limit to first 100 rows to prevent UI freeze
- Shows "... (X more rows not shown)" if truncated

**Execution Statistics:**
- Logs query execution time in milliseconds
- Shows row count and column count
- Example: "Query OK: 10 rows, 8 columns, 15 ms"

## Key Differences from Hour 2.2

| Aspect | Hour 2.2 | Hour 3 |
|--------|----------|--------|
| Query Input | None | Multi-line EDIT control |
| Execute Button | None | Yes |
| Query Execution | No | Yes (PQexec) |
| Result Display | No | Formatted table in debug window |
| Execution Time | No | Yes (GetTickCount) |
| NULL Handling | N/A | Shows "(NULL)" |
| Row Limit | N/A | 100 rows max display |
| Error Handling | Connection only | Connection + Query errors |

## Building

```bash
cd hour3_query_execution
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/17"
cmake --build . --config Release
Release\pqview.exe
```

## Layout Changes

Window height remains 700px, but internal layout adjusted:

```
┌─────────────────────────────────────────────────────────┐
│ ┌─ Connection Settings ─────────────────────────────┐   │
│ │ Host: [localhost] Port: [5432] ...                │   │
│ │ User: [postgres]  Password: [********]            │   │
│ │ Database: [stream_demo]                           │   │
│ └───────────────────────────────────────────────────┘   │
│ [Connect]             Status: Connected                 │
├─────────────────────────────────────────────────────────┤
│ ┌─ SQL Query ───────────────────────────────────────┐   │
│ │ SELECT * FROM customers LIMIT 10;                 │   │
│ │                                                    │   │
│ └───────────────────────────────────────────────────┘   │
│ [Execute Query]                                         │
├─────────────────────────────────────────────────────────┤
│ ┌─ Debug Output ────────────────────────────────────┐   │
│ │ [10:30:15] Application started                    │   │
│ │ [10:30:20] Connecting to localhost:5432...        │   │
│ │ [10:30:21] Connected to database 'stream_demo'    │   │
│ │ [10:30:25] Executing query...                     │   │
│ │ [10:30:25] Query: SELECT * FROM customers LIMIT 10│   │
│ │ [10:30:25] Query OK: 10 rows, 8 columns, 12 ms    │   │
│ │ customer_id          first_name           ...      │   │
│ │ -------------------- -------------------- ...      │   │
│ │ 1                    Alice                ...      │   │
│ │ 2                    Bob                  ...      │   │
│ └───────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

**Dimensions:**
- Connection Settings: 102px height (unchanged)
- SQL Query Group: 120px height (NEW)
- Debug Output Group: 350px height (reduced from 480px)

## New Windows API Concepts

### Multi-line EDIT Control

**Creating multi-line input:**
```c
CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
    WS_CHILD | WS_VISIBLE | WS_TABSTOP |
    ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL,
    x, y, width, height, ...);
```

**Styles explained:**
- `ES_MULTILINE` - Allow multiple lines
- `ES_AUTOVSCROLL` - Auto-scroll vertically when typing
- `ES_WANTRETURN` - Enter key adds newline (not default button)
- `WS_VSCROLL` - Show vertical scrollbar

### GetTickCount() for Timing

**Measuring execution time:**
```c
DWORD startTime = GetTickCount();  /* milliseconds since boot */
/* ... do work ... */
DWORD endTime = GetTickCount();
DWORD elapsed = endTime - startTime;
```

**Note:** GetTickCount() has ~15ms resolution on most systems.

### String Concatenation with strncat()

**Safe string building:**
```c
char buffer[4096] = "";
strncat(buffer, "text", sizeof(buffer) - strlen(buffer) - 1);
```

Always leave room for null terminator!

## PostgreSQL libpq Result Functions

### Result Status

**PQresultStatus():**
- Returns `ExecStatusType` enum
- `PGRES_TUPLES_OK` - SELECT query succeeded
- `PGRES_COMMAND_OK` - INSERT/UPDATE/DELETE succeeded
- `PGRES_FATAL_ERROR` - Query failed

### Result Dimensions

**PQntuples(res):**
- Returns number of rows returned by query
- For SELECT: actual row count
- For INSERT/UPDATE/DELETE: 0

**PQnfields(res):**
- Returns number of columns in result
- For SELECT: column count
- For INSERT/UPDATE/DELETE: 0

### Column Information

**PQfname(res, column_number):**
- Returns column name as `const char*`
- Column numbers are 0-indexed
- Example: `PQfname(res, 0)` returns first column name

### Cell Values

**PQgetvalue(res, row, col):**
- Returns cell value as `const char*`
- Always returns text representation
- Returns empty string "" for NULL (check with PQgetisnull first!)

**PQgetisnull(res, row, col):**
- Returns 1 if cell is NULL
- Returns 0 if cell has a value
- Always check before calling PQgetvalue for NULL handling

### Memory Management

**PQclear(res):**
- Frees memory allocated for result
- Must be called for every PQexec result
- Even if query fails, still call PQclear!

## Sample Test Queries

**Basic SELECT:**
```sql
SELECT * FROM customers LIMIT 10;
```

**With WHERE clause:**
```sql
SELECT first_name, last_name, email FROM customers WHERE city = 'New York';
```

**JOIN query:**
```sql
SELECT c.first_name, c.last_name, o.order_date
FROM customers c
JOIN orders o ON c.customer_id = o.customer_id
LIMIT 10;
```

**Aggregation:**
```sql
SELECT category_name, COUNT(*) as product_count
FROM products p
JOIN categories c ON p.category_id = c.category_id
GROUP BY category_name;
```

**Error test (invalid syntax):**
```sql
SELECT * FORM customers;
```

**Error test (table doesn't exist):**
```sql
SELECT * FROM nonexistent_table;
```

## Common Issues

### "Not connected to database" error

**Cause:** Forgot to click Connect button before Execute Query

**Solution:** Click Connect button first to establish database connection

### Query execution freezes the UI

**Cause:** Very large result set (>10000 rows)

**Solution:**
- Already limited to 100 rows in debug display
- For larger queries, add LIMIT clause to SQL

### Text doesn't fit in columns

**Cause:** Column width fixed at 20 characters

**Solution:**
- Values longer than 18 chars are truncated with "..."
- Hour 4 will use ListView for proper column sizing

### NULL values show as empty strings

**Fixed:** Hour 3 properly checks `PQgetisnull()` and displays "(NULL)"

## Error Handling

**Connection Check:**
```c
if (g_conn == NULL || PQstatus(g_conn) != CONNECTION_OK)
{
    MessageBox(hwnd, "Please connect to database first", "Error", MB_OK | MB_ICONERROR);
    return;
}
```

**Query Validation:**
```c
if (strlen(query) == 0)
{
    MessageBox(hwnd, "Please enter a SQL query", "Error", MB_OK | MB_ICONWARNING);
    return;
}
```

**Result Status Check:**
```c
if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK)
{
    MessageBox(hwnd, PQerrorMessage(g_conn), "Query Error", MB_OK | MB_ICONERROR);
    PQclear(res);
    return;
}
```

## Stream Talking Points

1. **Why multi-line EDIT?** - SQL queries often span multiple lines
2. **Why ES_WANTRETURN?** - Allows Enter to create newlines instead of clicking default button
3. **Why GetTickCount()?** - Simple millisecond timer, good enough for query timing
4. **Why limit to 100 rows?** - Prevent UI freeze with massive result sets
5. **Why 20-char columns?** - Fixed width for simple table layout, ListView in Hour 4 will be better
6. **Why check PQgetisnull?** - PQgetvalue returns "" for NULL, which looks like empty string
7. **Why both TUPLES_OK and COMMAND_OK?** - Some queries return data (SELECT), others don't (INSERT)

## Testing Checklist

- [ ] Connect to database
- [ ] Execute default query (customers)
- [ ] Clear query, enter new SELECT
- [ ] Test query with NULL values
- [ ] Test query with long text (truncation)
- [ ] Test JOINs
- [ ] Test aggregations (COUNT, SUM, etc.)
- [ ] Test invalid SQL syntax (should show error)
- [ ] Test nonexistent table (should show error)
- [ ] Execute query without connecting first (should show error)
- [ ] Execute empty query (should show error)

## Next Steps (Hour 4)

- Replace debug output table with ListView control
- Dynamic column headers in ListView
- Better column sizing (auto-size to content)
- Scroll through results properly
- Select individual rows
- Keep debug output for logging (separate from results)

## Comparison

**Hour 2.2 (Themes):**
- Connection UI only
- No query execution
- Debug window for logs only
- Modern XP+ styling

**Hour 3 (Query Execution):**
- Connection UI + Query input
- Execute queries with PQexec
- Display results in debug window (formatted table)
- Execution timing
- NULL handling
- Error handling for queries
- Same modern XP+ styling
