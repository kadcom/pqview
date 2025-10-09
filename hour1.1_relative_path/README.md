# Hour 1.1: Relative Path Linking

## Objectives

- Set up CMake with **manual relative path** linking to PostgreSQL
- Use `find_library()` and manual path configuration instead of `find_package()`
- Link to PostgreSQL using `PGSQL_DIR` variable
- Test PostgreSQL connection on startup

## Files

- `CMakeLists.txt` - CMake with manual relative path configuration
- `main.c` - Win32 application with PostgreSQL connection test

## What's Implemented

### CMakeLists.txt
- **Manual path configuration** instead of `find_package(PostgreSQL)`
- Uses `PGSQL_DIR` cache variable (can be set via command line or environment)
- Manually sets include and library directories
- Uses `find_library()` to locate libpq
- Explicit `target_link_directories()` for library path
- More control over PostgreSQL location

### main.c
- Same as Hour 1, but window title shows "Hour 1.1"
- PostgreSQL connection test on startup with MessageBox feedback

## Key Differences from Hour 1

| Aspect | Hour 1 (Absolute) | Hour 1.1 (Relative) |
|--------|-------------------|---------------------|
| Method | `find_package(PostgreSQL)` | Manual `find_library()` |
| Path Setting | `-DPostgreSQL_ROOT="C:/..."` | `-DPostgreSQL_ROOT="C:/..."` |
| Include Dir | Auto-detected | Manual: `${PostgreSQL_ROOT}/include` |
| Library Dir | Auto-detected | Manual: `${PostgreSQL_ROOT}/lib` |
| Library Find | Automatic | `find_library(... NAMES pq libpq)` |
| Control | CMake decides | You decide exactly |

## Building

### Prerequisites

1. **Visual Studio** with C/C++ development tools
2. **CMake** 3.20 or higher
3. **PostgreSQL** installed (version 17 or compatible)

### Build Steps

**Method 1: Specify PostgreSQL_ROOT on command line**
```bash
cd hour1.1_relative_path
mkdir build
cd build

# Configure with custom PostgreSQL directory (same as Hour 1!)
cmake .. -G "Visual Studio 17 2022" -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/17"

# Build
cmake --build . --config Release

# Run
Release\pqview.exe
```

**Method 2: Edit CMakeLists.txt default**
Edit line 11 in `CMakeLists.txt`:
```cmake
set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/17" CACHE PATH "PostgreSQL installation directory")
```

Then build:
```bash
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

**Method 3: Set environment variable**
```bash
# PowerShell
$env:PostgreSQL_ROOT = "C:/Program Files/PostgreSQL/17"
cmake .. -G "Visual Studio 17 2022"

# CMD
set PostgreSQL_ROOT=C:/Program Files/PostgreSQL/17
cmake .. -G "Visual Studio 17 2022"
```

## Configuration

### PostgreSQL Connection String

Edit `main.c` line 154 to match your PostgreSQL setup:

```c
const char* conninfo = "host=localhost port=5432 dbname=stream_demo user=postgres password=postgres";
```

## Expected Behavior

1. Window opens with title "pqview - Hour 1.1"
2. Text displays showing successful window creation
3. MessageBox appears showing PostgreSQL connection status

## CMake Debug Output

When you run cmake, you should see:

```
-- PostgreSQL root: C:/Program Files/PostgreSQL/17
-- PostgreSQL include dir: C:/Program Files/PostgreSQL/17/include
-- PostgreSQL library dir: C:/Program Files/PostgreSQL/17/lib
-- PostgreSQL library: C:/Program Files/PostgreSQL/17/lib/libpq.lib
```

## Why Use This Approach?

### Advantages of Manual Relative Paths:
1. **More control** - You explicitly set every path
2. **Portable** - Can package PostgreSQL with your project
3. **No CMake module dependency** - Don't rely on FindPostgreSQL.cmake
4. **Learning** - Understand exactly how linking works
5. **Flexibility** - Can use custom PostgreSQL builds

### When to Use Hour 1 vs Hour 1.1:
- **Hour 1** - Standard system-wide PostgreSQL installation
- **Hour 1.1** - Custom PostgreSQL location, portable builds, or learning purposes

## Troubleshooting

### "PostgreSQL library not found in..."

The library search failed. Check:
1. Is PostgreSQL installed at the specified `PostgreSQL_ROOT`?
2. Does `${PostgreSQL_ROOT}/lib` contain `libpq.lib` or `pq.lib`?
3. Try listing the directory:
   ```bash
   dir "C:\Program Files\PostgreSQL\17\lib\libpq*"
   ```

### Library is found but linking fails

Check that you're linking against the correct architecture:
- 64-bit PostgreSQL → 64-bit build (`-A x64`)
- 32-bit PostgreSQL → 32-bit build (`-A Win32`)

### Include files not found

Check that `${PostgreSQL_ROOT}/include` contains `libpq-fe.h`:
```bash
dir "C:\Program Files\PostgreSQL\17\include\libpq-fe.h"
```

## Key Concepts Covered

### CMake Manual Path Configuration
- **CACHE variables** - Persistent configuration values
- **find_library()** - Search for specific library files
- **target_link_directories()** - Add library search paths
- **NO_DEFAULT_PATH** - Restrict search to specified paths only
- **FATAL_ERROR** - Stop configuration if library not found

### Path Variables
- `${PostgreSQL_ROOT}` - Root PostgreSQL directory
- `${PostgreSQL_ROOT}/include` - Header files location
- `${PostgreSQL_ROOT}/lib` - Library files location

## Stream Talking Points

1. **Why manual paths?** - More control, understand the linking process
2. **What is find_library()?** - CMake function to search for library files
3. **What is CACHE?** - Persistent variables that can be set from command line
4. **Why NO_DEFAULT_PATH?** - Prevents CMake from searching system paths
5. **When would you use this?** - Custom builds, portable applications, learning

## Next Steps (Hour 2)

- Add connection parameter UI controls
- Add Connect button
- Create debug output window
- Store connection handle globally
- Display connection messages in debug window
