# Hour 5.2: Modular Architecture

This version refactors the PostgreSQL Query Client into a clean, modular architecture with separated concerns.

## Module Structure

### 1. **constants.h**
- Window class name
- Window dimensions
- Icon resource ID
- All control IDs (101-114)
- Keyboard shortcut IDs (1001-1003)

### 2. **debuglog.h / debuglog.c**
- `DebugLog_Init()` - Initialize debug window handle
- `DebugLog()` - Append timestamped messages to debug listbox

### 3. **database.h / database.c**
PostgreSQL operations module:
- `Database_Init()` - Initialize module
- `Database_Connect()` - Connect using ConnectionParams struct
- `Database_Disconnect()` - Disconnect and cleanup
- `Database_ExecuteQuery()` - Execute SQL and return PGresult
- `Database_CheckConnection()` - Validate connection status
- `Database_GetCurrentResult()` - Get cached result for virtual ListView
- `Database_SetCurrentResult()` - Update cached result
- `Database_Cleanup()` - Final cleanup of all resources
- `Database_IsConnected()` - Get connection state

### 4. **controls.h / controls.c**
UI control management module:
- `Controls_Create()` - Create all UI controls
- `Controls_Resize()` - Handle window resizing
- `Controls_UpdateConnectionState()` - Update UI based on connection
- `Controls_UpdateStatus()` - Update status label
- `Controls_UpdateRowCount()` - Update row count display
- `Controls_CreateFonts()` - Create application fonts
- `Controls_CleanupFonts()` - Cleanup fonts
- `Controls_GetHandles()` - Get global control handles

Structures:
- `ControlHandles` - All window control handles
- `FontHandles` - Application font handles

### 5. **clipboard.h / clipboard.c**
Clipboard operations module:
- `Clipboard_CopySelectedRow()` - Copy selected ListView row to clipboard

### 6. **main.c**
Main application logic:
- `WinMain()` - Application entry point
- `WindowProc()` - Window message handler
- `HandleConnect()` - Connect button handler
- `HandleDisconnect()` - Disconnect button handler
- `HandleExecuteQuery()` - Execute query handler
- `HandleClearResults()` - Clear results handler
- `HandleCopyResults()` - Copy handler
- `CreateAccelerators()` - Keyboard shortcuts

## Key Improvements

### Separation of Concerns
Each module has a clear, focused responsibility:
- Database module handles all PostgreSQL operations
- Controls module manages all UI creation and updates
- Clipboard module handles copy operations
- DebugLog module provides centralized logging
- Main module coordinates between modules

### Better Resource Management
- `WM_CLOSE` handler calls `Database_Cleanup()` before destroying window
- `WM_DESTROY` handler does final cleanup and font deletion
- Database module owns and manages PGresult lifecycle
- Clear initialization and cleanup paths

### Improved Maintainability
- Header guards prevent multiple inclusion
- Clear module interfaces with documented functions
- Structures group related data (ControlHandles, FontHandles, ConnectionParams)
- Static globals encapsulated within modules

### All Hour 5 Features Retained
- Keyboard shortcuts (F5, Ctrl+L, Ctrl+C)
- Clear results functionality
- Copy selected row to clipboard
- Row count display
- Virtual ListView with LVS_OWNERDATA
- Custom icon
- Proper font handling
- Connection state management

## Building

```bash
cd hour5.2_modular
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

Same as Hour 5.1 - all functionality preserved:
1. Enter connection details
2. Click "Connect" or press Enter
3. Enter SQL query
4. Press F5 or click "Execute Query"
5. View results in ListView
6. Use Ctrl+C to copy selected row
7. Use Ctrl+L to clear results

## Module Dependencies

```
main.c
├── constants.h
├── debuglog.h → debuglog.c
├── database.h → database.c → debuglog.h
├── controls.h → controls.c → constants.h
└── clipboard.h → clipboard.c → debuglog.h

database.c uses DebugLog()
clipboard.c uses DebugLog()
All modules include constants.h where needed
```

## File Count

- **Headers**: 5 (.h files)
- **Source**: 5 (.c files)
- **Resources**: 2 (app.rc, db.ico)
- **Build**: 1 (CMakeLists.txt)
- **Docs**: 1 (README.md)
- **Total**: 14 files

## Benefits of Modularization

1. **Testability** - Each module can be tested independently
2. **Reusability** - Modules can be reused in other projects
3. **Maintainability** - Changes are localized to specific modules
4. **Readability** - Smaller files with focused responsibilities
5. **Team Development** - Multiple developers can work on different modules
6. **Debugging** - Easier to trace issues to specific modules
