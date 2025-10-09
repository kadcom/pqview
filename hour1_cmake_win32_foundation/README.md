# Hour 1: CMake + Win32 Foundation

## Objectives

- Set up CMake project for pure C (MSVC)
- Link PostgreSQL libpq library
- Create basic Win32 window with WinMain
- Test PostgreSQL connection on startup
- Display connection result in MessageBox

## Files

- `CMakeLists.txt` - CMake configuration for pure C project (pqview)
- `main.c` - Win32 application with PostgreSQL connection test

## What's Implemented

### CMakeLists.txt
- Project name: **pqview**
- Pure C project (C11 standard)
- CMake 3.20 minimum version
- Finds PostgreSQL package
- Links libpq and Windows libraries (user32, gdi32, comctl32, ws2_32)
- WIN32 subsystem (GUI app)
- MSVC-specific compiler flags (/W4)

### main.c
- **WinMain** - Entry point for Windows GUI application
- **WindowProc** - Message handler for window events
- **TestPostgreSQLConnection** - Tests database connection using libpq
- Basic window with painted text
- Connection test on startup with MessageBox feedback

## Building

### Prerequisites

1. **Visual Studio** with C/C++ development tools
2. **CMake** 3.20 or higher
3. **PostgreSQL** installed with development files (libpq)

### Build Steps

```bash
# Navigate to hour1 directory
cd hour1_cmake_win32_foundation

# Create build directory
mkdir build
cd build

# Configure with CMake (if PostgreSQL not found automatically)
cmake .. -G "Visual Studio 17 2022" -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/16"

# Or if PostgreSQL is found automatically
cmake .. -G "Visual Studio 17 2022"

# Build
cmake --build . --config Release

# Run
Release\pqview.exe
```

## Configuration

### PostgreSQL Connection String

Edit `main.c` line 143 to match your PostgreSQL setup:

```c
const char* conninfo = "host=localhost port=5432 dbname=stream_demo user=postgres password=postgres";
```

## Expected Behavior

1. Window opens with title "pqview - Hour 1"
2. Text displays showing successful window creation
3. MessageBox appears showing PostgreSQL connection status

## Key Concepts Covered

### Win32 API
- WinMain entry point
- WNDCLASSEX window class registration
- CreateWindowEx window creation
- Message loop
- WindowProc message handling
- MessageBox for output

### PostgreSQL libpq
- PQconnectdb - Database connection
- PQstatus - Check connection status
- PQexec - Execute SQL query
- PQgetvalue - Get result data
- PQclear, PQfinish - Cleanup

### CMake
- Pure C project configuration
- find_package for PostgreSQL
- WIN32 executable type
- target_link_libraries

## Next Steps (Hour 2)

- Add connection parameter UI controls
- Add Connect button
- Create debug output window
- Store connection handle globally
