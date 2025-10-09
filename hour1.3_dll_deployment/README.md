# Hour 1.3: DLL Deployment

## Objectives

- Automatically copy PostgreSQL DLLs to executable directory during build
- Solve runtime DLL dependency issues
- Make the application portable (no PATH modification needed)
- Use CMake `add_custom_command` with `POST_BUILD`

## Files

- `CMakeLists.txt` - CMake with automatic DLL copying
- `main.c` - Win32 application with PostgreSQL connection test

## What's Implemented

### CMakeLists.txt
- Same as Hour 1.1 but with **automatic DLL deployment**
- Defines list of required PostgreSQL DLLs
- Uses `add_custom_command(POST_BUILD)` to copy DLLs
- Copies from `${PostgreSQL_ROOT}/bin` to output directory
- Uses `copy_if_different` to avoid unnecessary copies
- Runs after every successful build

### DLLs Copied
- `libpq.dll` - PostgreSQL client library
- `libssl-3-x64.dll` - SSL/TLS support
- `libcrypto-3-x64.dll` - Cryptography functions
- `libintl-9.dll` - Internationalization
- `libiconv-2.dll` - Character encoding conversion

### main.c
- Same as Hour 1.1, but window title shows "Hour 1.3"
- PostgreSQL connection test on startup

## Key Differences from Hour 1.1

| Aspect | Hour 1.1 | Hour 1.3 |
|--------|----------|----------|
| DLL Handling | Manual copy or PATH | Automatic copy |
| Build Output | Just .exe | .exe + all DLLs |
| Portability | Needs PATH or manual work | Fully portable |
| Runtime | May fail if DLLs not found | Always works |
| Distribution | Multi-step | Single folder |

## Building

### Prerequisites

1. **Visual Studio** with C/C++ development tools
2. **CMake** 3.20 or higher
3. **PostgreSQL** installed (version 17 or compatible)

### Build Steps

```bash
cd hour1.3_dll_deployment
mkdir build
cd build

# Configure
cmake .. -G "Visual Studio 17 2022" -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/17"

# Build
cmake --build . --config Release

# Run (DLLs are already in the same directory!)
Release\pqview.exe
```

### Expected Build Output

During build, you should see:
```
...
-- Copying libpq.dll to output directory
-- Copying libssl-3-x64.dll to output directory
-- Copying libcrypto-3-x64.dll to output directory
-- Copying libintl-9.dll to output directory
-- Copying libiconv-2.dll to output directory
Build completed successfully!
PostgreSQL DLLs copied to output directory
```

### Output Directory Contents

After building, `Release\` contains:
```
Release/
├── pqview.exe
├── libpq.dll
├── libssl-3-x64.dll
├── libcrypto-3-x64.dll
├── libintl-9.dll
└── libiconv-2.dll
```

## Configuration

### PostgreSQL Connection String

Edit `main.c` line 154 to match your PostgreSQL setup:

```c
const char* conninfo = "host=localhost port=5432 dbname=stream_demo user=postgres password=postgres";
```

## Expected Behavior

1. Window opens with title "pqview - Hour 1.3"
2. Text displays showing DLL deployment
3. MessageBox appears showing PostgreSQL connection status
4. **No DLL errors** - everything runs immediately!

## How DLL Copying Works

### CMake POST_BUILD Command

```cmake
foreach(dll ${PGSQL_DLLS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PostgreSQL_BIN_DIR}/${dll}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copying ${dll} to output directory"
    )
endforeach()
```

**Breakdown:**
- `POST_BUILD` - Runs after successful compilation and linking
- `copy_if_different` - Only copies if source is newer than destination
- `${PostgreSQL_BIN_DIR}/${dll}` - Source: `C:/Program Files/PostgreSQL/17/bin/libpq.dll`
- `$<TARGET_FILE_DIR:${PROJECT_NAME}>` - Destination: `build/Release/` (generator expression)
- `foreach` loop - Copies each DLL in the list

### Generator Expression `$<TARGET_FILE_DIR:...>`

This is a CMake **generator expression** that evaluates to the directory containing the built executable:
- Debug build → `build/Debug/`
- Release build → `build/Release/`
- MinSizeRel build → `build/MinSizeRel/`
- Automatically adjusts based on configuration

## Advantages

### 1. **No PATH Modification Required**
- User doesn't need to add PostgreSQL to system PATH
- Works on clean Windows installations
- No conflicts with other PostgreSQL versions

### 2. **Portable**
- Can zip the entire `Release/` folder
- Copy to another machine and run immediately
- Self-contained application

### 3. **Deployment-Ready**
- All dependencies in one place
- Easy to create installers
- Simple distribution

### 4. **Development-Friendly**
- Automatic - no manual steps
- Always up-to-date after rebuild
- Less room for error

## Troubleshooting

### "Could not copy file ... access denied"

The DLL might be in use (application still running). Close all instances of pqview.exe and rebuild.

### DLL not found even after copying

Check that the DLL actually exists in PostgreSQL's bin directory:
```bash
dir "C:\Program Files\PostgreSQL\17\bin\libpq.dll"
```

If missing, PostgreSQL installation might be incomplete.

### Wrong architecture (x64 vs x86)

Make sure your build architecture matches the DLLs:
```bash
# For 64-bit PostgreSQL
cmake .. -G "Visual Studio 17 2022" -A x64

# For 32-bit PostgreSQL (rare)
cmake .. -G "Visual Studio 17 2022" -A Win32
```

### Different PostgreSQL version has different DLL names

Edit the `PGSQL_DLLS` list in CMakeLists.txt to match your version:
```cmake
set(PGSQL_DLLS
    libpq.dll
    # Add or remove DLLs as needed for your PostgreSQL version
)
```

## Key Concepts Covered

### CMake Build Events
- **POST_BUILD** - Commands that run after compilation
- **PRE_BUILD** - Commands that run before compilation
- **PRE_LINK** - Commands that run before linking

### CMake File Operations
- **copy_if_different** - Smart copying (checks timestamps)
- **copy** - Always copies
- **copy_directory** - Copy entire directory trees

### Generator Expressions
- **$<TARGET_FILE_DIR:target>** - Output directory
- **$<TARGET_FILE:target>** - Full path to executable
- **$<CONFIG>** - Current build configuration (Debug/Release)

### Deployment Strategies
- **Static linking** - Compile everything into .exe (not possible with libpq)
- **DLL side-by-side** - What we're doing (best for libpq)
- **System PATH** - Install DLLs globally (inconvenient for users)

## Stream Talking Points

1. **Why copy DLLs?** - Solve the "DLL not found" runtime error
2. **Why not use PATH?** - More portable, no user configuration
3. **What is POST_BUILD?** - Automatic step after compilation
4. **Why copy_if_different?** - Faster incremental builds
5. **What about other dependencies?** - Same technique works for any DLL
6. **How does Windows find DLLs?** - Same directory first, then PATH

## Next Steps (Hour 2)

- Add connection parameter UI controls
- Add Connect button
- Create debug output window
- Store connection handle globally
- Display connection messages in debug window
- All DLLs are already deployed!
