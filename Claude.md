# PostgreSQL Query Client - Stream Project

A simple PostgreSQL SELECT query client built from scratch using CMake, Win32 API, and libpq.

## Stream Schedule (4-5 Hours)

### Hour 0: Basic Window
**Directory:** `hour0_basic_window/`
- Basic Win32 window with WinMain
- No PostgreSQL dependency
- Just Windows API (user32, gdi32)
- Foundation for adding features

### Hour 1: CMake + Win32 Foundation
**Directory:** `hour1_cmake_win32_foundation/`
- Set up CMake project structure
- Configure Win32 application
- Link PostgreSQL libpq library using find_package
- Test connection on startup
- Display version in MessageBox

### Hour 1.1: Relative Path Configuration
**Directory:** `hour1.1_relative_path/`
- Replace find_package with manual library paths
- Use PostgreSQL_ROOT for flexibility
- Manual find_library() approach
- Better control over library locations

### Hour 1.3: DLL Deployment
**Directory:** `hour1.3_dll_deployment/`
- Automated DLL copying with CMake POST_BUILD
- Copy libpq.dll and dependencies to output directory
- Single distributable executable folder

### Hour 2: Connection UI + Debug Window
**Directory:** `hour2_connection_debug/`
- Add connection parameter controls (Host, Port, User, Password, Database)
- Implement Connect button functionality
- Create scrollable EDIT debug output window
- Test PostgreSQL connection
- Display connection status
- **Note:** Has text rendering issues when scrolling

### Hour 2.1: Modern UI (Windows 95 Style)
**Directory:** `hour2.1_modern_ui/`
- **CRITICAL FIX:** Switch from EDIT to LISTBOX for debug output
- Fixes text rendering/overlap issues from Hour 2
- Add MS Shell Dlg font (8pt), Courier New monospace
- Group boxes for organization
- COLOR_BTNFACE background (gray dialog)
- WM_CTLCOLORSTATIC for transparent labels
- 3D borders with WS_EX_WINDOWEDGE and WS_EX_CLIENTEDGE
- Move Connect button outside group box
- Add tab order support

### Hour 2.2: Windows XP+ Themes
**Directory:** `hour2.2_xp_themes/`
- Embedded manifest for Common Controls v6
- InitCommonControlsEx() initialization
- Segoe UI font (Vista+) with fallback to Tahoma (XP)
- Consolas monospace font with fallback to Courier New
- CLEARTYPE_QUALITY for anti-aliased text
- Themed controls (gradient buttons, blue focus rectangles)
- Same layout as Hour 2.1, just modern visual styles
- Compatible with Windows XP through Windows 11

### Hour 3: Query Execution
**Directory:** `hour3_query_execution/` *(TODO)*
- Add multi-line query input control
- Implement Execute button
- Execute SELECT queries using libpq
- Display results in debug window
- Error handling and messages

### Hour 4: ListView Results
**Directory:** `hour4_listview_results/` *(TODO)*
- Create ListView control (LVS_REPORT mode)
- Dynamically add column headers from query results
- Populate rows with data
- Handle different data types
- Clear and refresh ListView

### Hour 5: Polish & Features
**Directory:** `hour5_polish_features/` *(TODO)*
- Handle NULL values display
- Better error messages
- Column auto-sizing
- Timestamps in debug output (already in Hour 2.1+)
- Row/execution statistics
- Final testing and demo

## Database Setup

### 1. Install PostgreSQL
Make sure PostgreSQL is installed and running locally.

### 2. Create Database
```bash
createdb stream_demo
```

Or using psql:
```sql
CREATE DATABASE stream_demo;
```

### 3. Initialize Schema and Data
```bash
psql -d stream_demo -f init_database.sql
```

### 4. Verify Setup
```bash
psql -d stream_demo -c "SELECT COUNT(*) FROM customers;"
```

## Database Schema

The demo database includes:

- **categories** - Product categories (5 records)
- **products** - Products with prices and stock (12 records)
- **customers** - Customer information (10 records)
- **orders** - Customer orders (10 records)
- **order_items** - Order line items (14 records)

Plus two views:
- **customer_order_summary** - Customer purchase statistics
- **product_sales** - Product sales summary

## Sample Test Queries

See `sample_queries.sql` for ready-to-use queries organized by complexity:
- Basic SELECT statements
- JOINs and aggregations
- Subqueries and CASE statements
- Date/string functions
- Error cases for testing

## Connection Details (Default)

- **Host:** localhost
- **Port:** 5432
- **Database:** stream_demo
- **User:** postgres (or your PostgreSQL user)
- **Password:** (your PostgreSQL password)

## Build Requirements

- CMake 3.15+
- PostgreSQL with libpq development files
- Windows SDK (for Win32 API)
- C++ compiler (MSVC recommended on Windows)

## Project Structure

```
pq/
├── hour0_basic_window/               # Basic Win32 window (no PostgreSQL)
├── hour1_cmake_win32_foundation/     # PostgreSQL connection test
├── hour1.1_relative_path/            # Manual library paths (no find_package)
├── hour1.3_dll_deployment/           # Automated DLL copying
├── hour2_connection_debug/           # Connection UI + EDIT debug window
├── hour2.1_modern_ui/                # Windows 95 style + LISTBOX (fixes rendering)
├── hour2.2_xp_themes/                # Windows XP+ visual styles
├── hour3_query_execution/            # (TODO) Query input and execution
├── hour4_listview_results/           # (TODO) ListView results display
├── hour5_polish_features/            # (TODO) Polish and features
├── init_database.sql                 # Database setup script
├── sample_queries.sql                # Test queries
├── Claude.md                         # This file
└── README.md                         # Project documentation
```

## Application Layout

```
┌─────────────────────────────────────────────────────────┐
│ PostgreSQL Query Client                                 │
├─────────────────────────────────────────────────────────┤
│ Connection Settings:                                    │
│ Host: [localhost    ] Port: [5432] User: [postgres   ] │
│ Password: [********] Database: [stream_demo]           │
│ [Connect]                          Status: Connected    │
├─────────────────────────────────────────────────────────┤
│ SQL Query:                                              │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ SELECT * FROM customers                             │ │
│ │ WHERE city = 'New York'                             │ │
│ │ ORDER BY last_name;                                 │ │
│ └─────────────────────────────────────────────────────┘ │
│ [Execute Query]                                         │
├─────────────────────────────────────────────────────────┤
│ Results (ListView - LVS_REPORT):                        │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ customer_id │ first_name │ last_name │ email      │ │
│ │ 1           │ Alice      │ Johnson   │ alice@...  │ │
│ │ 8           │ Henry      │ Moore     │ henry@...  │ │
│ └─────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────┤
│ Debug Output:                                           │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ [10:30:15] Connecting to localhost:5432...          │ │
│ │ [10:30:16] Connected to database 'stream_demo'      │ │
│ │ [10:30:20] Executing query...                       │ │
│ │ [10:30:20] Query OK: 2 rows, 8 columns             │ │
│ │ [10:30:20] Execution time: 12ms                     │ │
│ │ [10:30:20] Displaying results in ListView           │ │
│ └─────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

## Windows Controls Used

- **Edit Controls** (connection params, query input, debug output)
  - Single-line: `ES_LEFT | WS_BORDER`
  - Multi-line: `ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL`
  - Read-only debug: `ES_READONLY`

- **Button Controls** (Connect, Execute)
  - Standard: `BS_PUSHBUTTON`

- **Static Controls** (labels)
  - Text labels: `SS_LEFT`

- **ListView Control** (results table)
  - Report mode: `LVS_REPORT | LVS_SINGLESEL | WS_BORDER`

## PostgreSQL libpq Functions

### Connection:
- `PQconnectdb(const char *conninfo)` - Connect to database
- `PQstatus(PGconn *conn)` - Check connection status
- `PQerrorMessage(PGconn *conn)` - Get error message
- `PQfinish(PGconn *conn)` - Close connection

### Query Execution:
- `PQexec(PGconn *conn, const char *query)` - Execute query
- `PQresultStatus(PGresult *res)` - Check query result status
- `PQresultErrorMessage(PGresult *res)` - Get query error

### Result Processing:
- `PQntuples(PGresult *res)` - Get number of rows
- `PQnfields(PGresult *res)` - Get number of columns
- `PQfname(PGresult *res, int column)` - Get column name
- `PQgetvalue(PGresult *res, int row, int column)` - Get cell value
- `PQgetisnull(PGresult *res, int row, int column)` - Check if NULL
- `PQclear(PGresult *res)` - Free result memory

## Stream Tips

1. **Copy forward each hour** - Start new hour by copying previous directory
2. **Test frequently** - Compile and run after each feature
3. **Use sample queries** - Copy from `sample_queries.sql` to avoid typos
4. **Show errors** - Intentionally trigger errors to demo error handling
5. **Debug window is key** - Narrate what's happening for viewers
6. **Incremental building** - Add one control at a time, test, then continue

## Learning Objectives

- CMake project configuration from scratch
- Linking external libraries (libpq)
- Win32 API fundamentals (WinMain, WNDCLASS, message loop)
- Window controls (Edit, Button, Static, ListView)
- Message handling (WM_COMMAND, WM_NOTIFY)
- ListView in report mode (columns, rows, dynamic data)
- PostgreSQL C API (libpq) basics
- SQL query execution and result processing
- Memory management (PQclear, proper cleanup)
- Error handling in native applications

## Common Issues & Solutions

### CMake can't find PostgreSQL:
```cmake
set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/16")
find_package(PostgreSQL REQUIRED)
```

### ListView columns not showing:
```cpp
InitCommonControls(); // Call in WinMain before creating window
```

### Debug window not scrolling:
```cpp
SendMessage(hwndDebug, EM_SCROLLCARET, 0, 0); // After appending text
```

### Connection string format:
```cpp
"host=localhost port=5432 dbname=stream_demo user=postgres password=yourpass"
```

## Extension Ideas (If Time Permits)

- Save query history
- Export results to CSV
- Syntax highlighting in query box
- Connection presets/favorites
- Query execution time display
- Row count limit option
- Multiple result tabs
- Dark mode toggle
