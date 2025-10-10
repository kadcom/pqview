# Hour 5.1: Custom Icon

This iteration adds a custom application icon using Windows resource files.

## What's New

### Custom Application Icon

Instead of using the default Windows application icon, this version uses a custom icon (`db.ico`) that appears in:
- Window title bar
- Taskbar
- Alt+Tab switcher
- Application executable file

## Technical Implementation

### Resource File (app.rc)

Created a Windows resource file that defines the icon:

```c
// Application resources
#include <windows.h>

// Application icon
#define IDI_PQVIEW 101

IDI_PQVIEW ICON "db.ico"
```

### CMakeLists.txt Changes

1. Added `RC` (Resource Compiler) language to project:
```cmake
project(pqview VERSION 1.0.0 LANGUAGES C RC)
```

2. Added `app.rc` to source files:
```cmake
set(SOURCES
    main.c
    app.rc
)
```

### main.c Changes

1. Added icon resource ID definition:
```c
#define IDI_PQVIEW 101
```

2. Updated LoadIcon calls to use custom icon:
```c
wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PQVIEW));
wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PQVIEW));
```

**Key difference:** Changed from `LoadIcon(NULL, IDI_APPLICATION)` (system icon) to `LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PQVIEW))` (custom icon from resources).

## Files Structure

```
hour5.1_custom_icon/
├── main.c           # C source with IDI_PQVIEW definition
├── app.rc           # Resource script defining icon
├── db.ico           # Icon file (provided by user)
└── CMakeLists.txt   # Build configuration with RC language
```

## Building

**Before building:** Make sure `db.ico` is in the `hour5.1_custom_icon/` directory.

```bash
cd hour5.1_custom_icon
mkdir build
cd build
cmake ..
cmake --build .
```

The resource compiler (rc.exe on Windows) will automatically compile `app.rc` and link it into the executable.

## What's New Since Hour 5

| Feature | Hour 5 | Hour 5.1 |
|---------|--------|----------|
| Application icon | Default Windows icon | Custom db.ico |
| Resource file | ❌ | ✅ app.rc |
| RC language in CMake | ❌ | ✅ Enabled |
| IDI_PQVIEW definition | ❌ | ✅ ID 101 |

## Windows Resource Compiler

### How it works:

1. **rc.exe** compiles `app.rc` → `app.res` (binary resource file)
2. **Linker** embeds `app.res` into `pqview.exe`
3. **LoadIcon()** extracts icon from executable at runtime

### MAKEINTRESOURCE Macro:

Converts integer ID to resource pointer:
```c
MAKEINTRESOURCE(101) → (LPSTR)((ULONG_PTR)((WORD)(101)))
```

This allows LoadIcon to accept both string names and integer IDs.

## Icon Requirements

- **Format:** .ico file
- **Recommended sizes:** 16x16, 32x32, 48x48, 256x256
- **Color depth:** 32-bit (with alpha channel for best results)
- **Location:** Same directory as app.rc

## Verifying the Icon

After building:
1. Check executable in File Explorer - should show custom icon
2. Run application - custom icon in title bar and taskbar
3. Alt+Tab while app is running - custom icon in task switcher

## Common Issues

### Icon not showing:

**Problem:** Still seeing default Windows icon

**Solutions:**
- Rebuild the project completely (delete build folder)
- Check that `db.ico` exists in source directory
- Verify rc.exe compiled app.rc (check build output)
- Clear icon cache: `ie4uinit.exe -show` (Windows 10/11)

### Build error "cannot open file 'db.ico'":

**Problem:** Resource compiler can't find icon file

**Solutions:**
- Place `db.ico` in `hour5.1_custom_icon/` directory (same as app.rc)
- Check filename matches exactly (case-sensitive on some systems)
- Verify path in app.rc is correct

### "RC.exe not found" error:

**Problem:** Resource compiler not in PATH

**Solutions:**
- Use Visual Studio Developer Command Prompt
- Install Windows SDK
- CMake usually finds rc.exe automatically with MSVC

## What Happened to Hour 5 Features?

All features from Hour 5 are still present:
- ✅ Keyboard shortcuts (F5, Ctrl+L, Ctrl+C)
- ✅ Clear Results button
- ✅ Copy Selected Row
- ✅ Row count display
- ✅ All polish features

**Hour 5.1 only adds:** Custom icon resource integration

## Stream Talking Points

1. **Why .rc files?** - Standard Windows way to embed resources (icons, strings, dialogs) into executables
2. **Why MAKEINTRESOURCE?** - Converts integer ID to format LoadIcon expects
3. **Why hInstance not NULL?** - NULL loads system icons, hInstance loads our embedded resources
4. **Why ID 101?** - Convention to start custom resources at 100+ (system IDs are below 100)
5. **Why both hIcon and hIconSm?** - hIcon for window/taskbar, hIconSm for title bar (small)
6. **CMake handles it automatically?** - Yes! Just add RC language and .rc file to sources
