# Hour 4.1: Connection State Management

## Objectives

- Implement Connect/Disconnect toggle button
- Enable/disable connection fields based on state
- Enable Execute Query button only when connected
- Handle connection loss detection
- Clear results on disconnect
- Provide proper user feedback for connection states
- Prevent actions when not connected

## Files

- `CMakeLists.txt` - Same as Hour 4
- `main.c` - Enhanced with connection state management

## What's New from Hour 4

### Connection State Management

**g_isConnected Flag:**
- Global boolean tracking connection state
- Updated by `UpdateConnectionState()` function
- Checked before executing queries

**Connect/Disconnect Toggle:**
- Button text changes: "Connect" ↔ "Disconnect"
- Click handler checks `g_isConnected` to determine action
- Single button for both operations

**UI State Updates:**
- Connection fields enabled/disabled based on state
- Execute Query button enabled only when connected
- Status updates reflect current state

### Connection Loss Detection

**CheckConnection() Function:**
- Validates connection exists (`g_conn != NULL`)
- Checks connection status with `PQstatus()`
- Detects server disconnection or network failure
- Automatically updates UI to disconnected state
- Shows error message to user

**Called Before Query Execution:**
- Every query execution calls `CheckConnection()`
- Prevents attempts to query disconnected database
- Gracefully handles mid-operation disconnects

### Disconnect Functionality

**HandleDisconnect() Function:**
- Closes database connection with `PQfinish()`
- Clears cached result (`g_currentResult`)
- Clears ListView (removes columns and items)
- Updates UI to disconnected state
- Logs disconnection event

### UI Control States

**When Connected:**
- Connection fields: **DISABLED**
- Connect button: Shows "**Disconnect**"
- Execute Query button: **ENABLED**
- Status: "Connected"

**When Disconnected:**
- Connection fields: **ENABLED**
- Connect button: Shows "**Connect**"
- Execute Query button: **DISABLED**
- Status: "Disconnected"

## Key Differences from Hour 4

| Aspect | Hour 4 | Hour 4.1 |
|--------|--------|----------|
| Connect Button | Connect only | Connect/Disconnect toggle |
| Connection Fields | Always enabled | Disabled when connected |
| Execute Button | Always enabled | Enabled only when connected |
| Connection Loss | Not detected | Detected and handled |
| Disconnect Action | Close window only | Explicit disconnect button |
| State Management | None | Full state tracking |
| UI Feedback | Basic | Clear state indication |

## Building

```bash
cd hour4.1_connection_state
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/17"
cmake --build . --config Release
Release\pqview.exe
```

## New Functions

### UpdateConnectionState()

**Purpose:** Centralized UI state management based on connection status

**Signature:**
```c
void UpdateConnectionState(HWND hwnd, BOOL connected)
```

**Actions when connected = TRUE:**
- Sets `g_isConnected = TRUE`
- Disables: Host, Port, User, Password, Database fields
- Changes button text to "Disconnect"
- Enables Execute Query button
- Updates status to "Connected"

**Actions when connected = FALSE:**
- Sets `g_isConnected = FALSE`
- Enables: Host, Port, User, Password, Database fields
- Changes button text to "Connect"
- Disables Execute Query button
- Updates status to "Disconnected"

**Called from:**
- `HandleConnect()` - On successful connection
- `HandleConnect()` - On connection failure
- `HandleDisconnect()` - After disconnection
- `CheckConnection()` - On connection loss detection

### HandleDisconnect()

**Purpose:** Disconnect from database and cleanup

**Signature:**
```c
void HandleDisconnect(HWND hwnd)
```

**Actions:**
1. Close connection with `PQfinish()`
2. Clear `g_currentResult` with `PQclear()`
3. Clear ListView (delete all items and columns)
4. Call `UpdateConnectionState(hwnd, FALSE)`
5. Log "Disconnected from database"

**Called from:**
- `WM_COMMAND` handler when Connect button clicked while connected

### CheckConnection()

**Purpose:** Validate connection is active and handle disconnection

**Signature:**
```c
BOOL CheckConnection(HWND hwnd)
```

**Returns:**
- `TRUE` - Connection is valid and active
- `FALSE` - Connection is invalid or lost

**Checks:**
1. `g_conn == NULL` → Show error, update UI, return FALSE
2. `PQstatus(g_conn) != CONNECTION_OK` → Detect server down/network issue
3. On connection loss:
   - Show "Connection to database has been lost" message
   - Cleanup connection with `PQfinish()`
   - Clear cached result
   - Call `UpdateConnectionState(hwnd, FALSE)`
   - Return FALSE

**Called from:**
- `HandleExecuteQuery()` - Before executing any query

## Implementation Details

### EnableWindow() API

**Syntax:**
```c
BOOL EnableWindow(HWND hWnd, BOOL bEnable)
```

**Parameters:**
- `hWnd` - Handle to window/control
- `bEnable` - TRUE to enable, FALSE to disable

**Effect:**
- Disabled controls appear grayed out
- Disabled controls don't respond to input
- Tab key skips disabled controls

**Example:**
```c
EnableWindow(g_hwndExecute, FALSE);  /* Disable Execute button */
EnableWindow(g_hwndHost, TRUE);      /* Enable Host field */
```

### SetWindowText() for Button Labels

**Changing button text:**
```c
SetWindowText(g_hwndConnect, "Disconnect");  /* Change to Disconnect */
SetWindowText(g_hwndConnect, "Connect");     /* Change back to Connect */
```

**Button automatically resizes (if needed):**
- Text centered in button
- Button width remains constant
- Font remains same

### Connection State Flow

**Initial State:**
```
[Disconnected]
Connection fields: ENABLED
Connect button: "Connect"
Execute button: DISABLED
```

**User clicks Connect → Success:**
```
HandleConnect()
  ↓
PQconnectdb() → CONNECTION_OK
  ↓
UpdateConnectionState(hwnd, TRUE)
  ↓
[Connected]
Connection fields: DISABLED
Connect button: "Disconnect"
Execute button: ENABLED
```

**User clicks Disconnect:**
```
HandleDisconnect()
  ↓
PQfinish()
Clear g_currentResult
Clear ListView
  ↓
UpdateConnectionState(hwnd, FALSE)
  ↓
[Disconnected]
```

**User clicks Execute → Connection Lost:**
```
HandleExecuteQuery()
  ↓
CheckConnection()
  ↓
PQstatus(g_conn) != CONNECTION_OK
  ↓
Show error message
PQfinish()
Clear results
  ↓
UpdateConnectionState(hwnd, FALSE)
  ↓
[Disconnected]
```

## Testing Scenarios

### Normal Connection Flow
- [ ] Start application - Execute button should be disabled
- [ ] Enter connection details
- [ ] Click Connect - Should connect successfully
- [ ] Connection fields should be disabled (grayed out)
- [ ] Button should change to "Disconnect"
- [ ] Execute button should be enabled
- [ ] Execute a query - Should work
- [ ] Click Disconnect - Should disconnect
- [ ] Connection fields should be enabled again
- [ ] Button should change to "Connect"
- [ ] Execute button should be disabled

### Connection Failure
- [ ] Enter wrong password
- [ ] Click Connect - Should show error
- [ ] Button should remain "Connect"
- [ ] Connection fields should remain enabled
- [ ] Execute button should remain disabled
- [ ] Status should show "Connection Failed"

### Connection Loss Detection
- [ ] Connect to database successfully
- [ ] Stop PostgreSQL server (or disconnect network)
- [ ] Try to execute query
- [ ] Should detect connection loss
- [ ] Should show "Connection to database has been lost" error
- [ ] UI should return to disconnected state
- [ ] Execute button should be disabled

### UI State Consistency
- [ ] After connection failure - UI should be in disconnected state
- [ ] After disconnect - ListView should be cleared
- [ ] After connection loss - Previous results should be cleared
- [ ] Can't modify connection fields while connected
- [ ] Can't execute queries while disconnected

## Common Issues

### Execute button remains enabled after connection loss

**Cause:** Forgot to call `UpdateConnectionState(hwnd, FALSE)` in `CheckConnection()`

**Solution:** Always update UI state when connection is invalidated

### Can still type in connection fields when connected

**Cause:** Forgot to disable fields in `UpdateConnectionState()`

**Solution:** Call `EnableWindow(g_hwndHost, FALSE)` etc. when connected

### Disconnect doesn't clear results

**Cause:** Forgot to clear ListView in `HandleDisconnect()`

**Solution:** Call `ListView_DeleteAllItems()` and delete columns

### Connection loss not detected

**Cause:** Not calling `CheckConnection()` before operations

**Solution:** Call `CheckConnection()` at start of `HandleExecuteQuery()`

### Memory leak on disconnect

**Cause:** Not clearing `g_currentResult` before `PQfinish()`

**Solution:** Always `PQclear(g_currentResult)` before disconnecting

## Error Messages

**"Please connect to database first":**
- Shown when: User tries to execute query without connection
- Action: Click Connect button first

**"Connection to database has been lost":**
- Shown when: Server goes down or network disconnected during use
- Action: Fix server/network issue, then reconnect

**"Connection FAILED: [error details]":**
- Shown when: Initial connection attempt fails
- Action: Check connection parameters (host, port, user, password)

## Stream Talking Points

1. **Why toggle button?** - Single button is cleaner than two separate buttons
2. **Why disable fields?** - Prevents confusion, can't change connection while connected
3. **Why check before query?** - Server could go down between connection and query
4. **Why clear results on disconnect?** - Results are invalid after disconnection
5. **Why EnableWindow()?** - Standard Windows way to show unavailable controls
6. **Why global state flag?** - Need to track state across multiple functions
7. **Why PQstatus()?** - Detects connection loss without attempting operation

## Next Steps (Hour 5)

- Save/load connection profiles
- Connection history
- Query history and favorites
- Export results to CSV
- Column sorting in ListView
- Copy cells to clipboard
- Keyboard shortcuts
- Polish and refinements

## Comparison

**Hour 4 (ListView Results):**
- No connection state management
- Connect button always says "Connect"
- Execute button always enabled
- No disconnection functionality
- Connection loss not detected
- Can modify connection fields anytime

**Hour 4.1 (Connection State):**
- Full connection state tracking with `g_isConnected`
- Connect/Disconnect toggle button
- Execute button enabled only when connected
- Explicit disconnect functionality
- Connection loss detection with `CheckConnection()`
- Connection fields disabled when connected
- Clear UI feedback for all states
- Graceful handling of server failures
- Professional state management
