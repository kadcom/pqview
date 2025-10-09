# Hour 0: Basic Win32 Window

## Objectives

- Set up CMake project for pure C (MSVC)
- Create basic Win32 window with WinMain
- Handle basic window messages (WM_CREATE, WM_PAINT, WM_DESTROY)
- No external dependencies - only Windows libraries

## Files

- `CMakeLists.txt` - Minimal CMake configuration
- `main.c` - Basic Win32 window application

## What's Implemented

### CMakeLists.txt
- Project name: **pqview**
- Pure C project (C11 standard)
- CMake 3.20 minimum version
- Links only Windows libraries (user32, gdi32)
- WIN32 subsystem (GUI app)
- WIN32_LEAN_AND_MEAN defined
- MSVC-specific compiler flags (/W4)

### main.c
- **WinMain** - Entry point for Windows GUI application
- **WindowProc** - Message handler for window events
- **WM_CREATE** - Window initialization
- **WM_PAINT** - Drawing text on the window
- **WM_DESTROY** - Clean shutdown
- Basic text rendering to show the window is working

## Building

### Prerequisites

1. **Visual Studio** with C/C++ development tools
2. **CMake** 3.20 or higher

### Build Steps

```bash
# Navigate to hour0 directory
cd hour0_basic_window

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 17 2022"

# Build
cmake --build . --config Release

# Run
Release\pqview.exe
```

### Alternative: Using Developer Command Prompt

```bash
# Open "x64 Native Tools Command Prompt for VS 2022"
cd hour0_basic_window
mkdir build
cd build

# Configure and build
cmake ..
cmake --build . --config Release

# Run
Release\pqview.exe
```

## Expected Behavior

1. Window opens with title "pqview - Hour 0"
2. Window displays three lines of centered text:
   - "pqview - Hour 0: Basic Window"
   - "CMake + Win32 API Foundation"
   - "Close this window to exit."
3. Window can be moved, resized, minimized, maximized
4. Closing the window exits the application

## Key Concepts Covered

### Win32 API Basics
- **WinMain** - GUI application entry point (not main)
- **WNDCLASSEX** - Window class registration structure
- **RegisterClassEx** - Register window class with Windows
- **CreateWindowEx** - Create a window instance
- **ShowWindow / UpdateWindow** - Display the window
- **Message Loop** - GetMessage, TranslateMessage, DispatchMessage
- **WindowProc** - Callback function for handling window messages
- **WM_CREATE** - Sent when window is created
- **WM_PAINT** - Sent when window needs to be redrawn
- **WM_DESTROY** - Sent when window is being destroyed
- **BeginPaint / EndPaint** - Drawing context management
- **DrawText** - Simple text rendering

### CMake Basics
- **project()** with LANGUAGES C
- **add_executable()** with WIN32 flag
- **target_link_libraries()** to link Windows libraries
- **target_compile_definitions()** for preprocessor defines
- **target_compile_options()** for compiler flags

### C Programming
- Function pointers (for WindowProc)
- Switch statements for message handling
- Structures (WNDCLASSEX, MSG, PAINTSTRUCT, RECT)
- Windows-specific types (HWND, HINSTANCE, WPARAM, LPARAM)

## Stream Talking Points

1. **Why WinMain?** - Windows GUI apps use WinMain instead of main()
2. **What is a message loop?** - Windows is event-driven; all input becomes messages
3. **Why register a window class?** - Defines common properties for window types
4. **What is a window procedure?** - Callback function that handles all messages for a window
5. **Why BeginPaint/EndPaint?** - Validates the window's update region
6. **What is WIN32_LEAN_AND_MEAN?** - Excludes rarely-used Windows headers, faster compilation

## Common Issues

### "undefined reference to WinMain"
- Make sure you have `WIN32` flag in `add_executable(${PROJECT_NAME} WIN32 ${SOURCES})`

### Window doesn't appear
- Check that `ShowWindow()` and `UpdateWindow()` are called
- Verify message loop is running

### Text doesn't show
- Make sure you handle `WM_PAINT` message
- Check that `BeginPaint()` and `EndPaint()` are properly paired

## Next Steps (Hour 1)

- Add PostgreSQL libpq to CMake
- Test database connection on startup
- Display connection result in MessageBox
- Verify all dependencies are working before building UI
