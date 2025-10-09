# PostgreSQL Query Client - Stream Project

A simple PostgreSQL SELECT query client built from scratch using CMake, Win32 API, and libpq.

## Stream Schedule (4-5 Hours)

### Hour 1: CMake + Win32 Foundation
**Directory:** `hour1_cmake_win32_foundation/`
- Set up CMake project structure
- Configure Win32 application
- Link PostgreSQL libpq library
- Create basic window with WinMain
- Test compilation and window creation

### Hour 2: Connection UI + Debug Window
**Directory:** `hour2_connection_debug/`
- Add connection parameter controls (Host, Port, User, Password, Database)
- Implement Connect button functionality
- Create scrollable debug output window
- Test PostgreSQL connection
- Display connection status in debug window

### Hour 3: Query Execution
**Directory:** `hour3_query_execution/`
- Add multi-line query input control
- Implement Execute button
- Execute SELECT queries using libpq
- Display results in debug window
- Error handling and messages

### Hour 4: ListView Results
**Directory:** `hour4_listview_results/`
- Create ListView control (LVS_REPORT mode)
- Dynamically add column headers from query results
- Populate rows with data
- Handle different data types
- Clear and refresh ListView

### Hour 5: Polish & Features
**Directory:** `hour5_polish_features/`
- Handle NULL values display
- Better error messages
- Column auto-sizing
- Timestamps in debug output
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
├── hour1_cmake_win32_foundation/
├── hour2_connection_debug/
├── hour3_query_execution/
├── hour4_listview_results/
├── hour5_polish_features/
├── init_database.sql          # Database setup script
├── sample_queries.sql         # Test queries
└── README.md                  # This file
```

## Stream Tips

1. Start each hour by copying previous hour's code
2. Test frequently - run after each major change
3. Use sample_queries.sql to avoid typing SQL live
4. The debug window helps viewers understand what's happening
5. Intentionally trigger errors to show error handling

## Learning Objectives

- CMake project configuration from scratch
- Win32 API fundamentals (windows, controls, message handling)
- ListView control in report mode
- PostgreSQL C API (libpq) basics
- SQL query execution and result processing
- Error handling in native applications
