/* controls.c - UI control creation and management implementation */

#include "controls.h"
#include "constants.h"
#include <commctrl.h>
#include <stdio.h>

/* Global control handles for internal use */
static ControlHandles g_handles = {0};

/* Create all UI controls */
void Controls_Create(HWND hwnd, ControlHandles* handles, const FontHandles* fonts)
{
    HWND hwndControl;
    int x = 12, y = 12;
    int labelWidth = 70;
    int fieldWidth = 160;
    int spacing = 26;

    /* Connection Settings Group Box */
    handles->hwndConnectionGroup = CreateWindowEx(0, "BUTTON", "Connection Settings",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        x, y, 860, 102, hwnd, NULL, NULL, NULL);
    SendMessage(handles->hwndConnectionGroup, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    x += 10;
    y += 18;

    /* Host */
    hwndControl = CreateWindowEx(0, "STATIC", "Host:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x, y, labelWidth, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    handles->hwndHost = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "localhost",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 21, hwnd, (HMENU)ID_EDIT_HOST, NULL, NULL);
    SendMessage(handles->hwndHost, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    /* Port */
    hwndControl = CreateWindowEx(0, "STATIC", "Port:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x + labelWidth + fieldWidth + 15, y, 40, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    handles->hwndPort = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "5432",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth + fieldWidth + 55, y, 70, 21, hwnd, (HMENU)ID_EDIT_PORT, NULL, NULL);
    SendMessage(handles->hwndPort, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    y += spacing;

    /* User */
    hwndControl = CreateWindowEx(0, "STATIC", "User:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x, y, labelWidth, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    handles->hwndUser = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "postgres",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 21, hwnd, (HMENU)ID_EDIT_USER, NULL, NULL);
    SendMessage(handles->hwndUser, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    /* Password */
    hwndControl = CreateWindowEx(0, "STATIC", "Password:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x + labelWidth + fieldWidth + 15, y, 60, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    handles->hwndPassword = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "postgres",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_PASSWORD | ES_AUTOHSCROLL,
        x + labelWidth + fieldWidth + 75, y, 160, 21, hwnd, (HMENU)ID_EDIT_PASSWORD, NULL, NULL);
    SendMessage(handles->hwndPassword, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    y += spacing;

    /* Database */
    hwndControl = CreateWindowEx(0, "STATIC", "Database:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x, y, labelWidth, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    handles->hwndDatabase = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "stream_demo",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 21, hwnd, (HMENU)ID_EDIT_DATABASE, NULL, NULL);
    SendMessage(handles->hwndDatabase, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    /* Reset x, y for controls below group box */
    x = 12;
    y = 122;

    /* Connect button */
    handles->hwndConnect = CreateWindowEx(0, "BUTTON", "Connect",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,
        x, y, 100, 24, hwnd, (HMENU)ID_BUTTON_CONNECT, NULL, NULL);
    SendMessage(handles->hwndConnect, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    /* Status */
    handles->hwndStatus = CreateWindowEx(0, "STATIC", "Status: Disconnected",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x + 115, y + 3, 300, 20, hwnd, (HMENU)ID_STATIC_STATUS, NULL, NULL);
    SendMessage(handles->hwndStatus, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    /* Reset x, y for query section */
    x = 12;
    y = 156;

    /* SQL Query Group Box */
    handles->hwndQueryGroup = CreateWindowEx(0, "BUTTON", "SQL Query",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        x, y, 860, 120, hwnd, NULL, NULL, NULL);
    SendMessage(handles->hwndQueryGroup, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    x += 10;
    y += 20;

    /* Query input - multi-line EDIT control */
    handles->hwndQuery = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "SELECT * FROM customers LIMIT 10;",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL,
        x, y, 840, 65, hwnd, (HMENU)ID_EDIT_QUERY, NULL, NULL);
    SendMessage(handles->hwndQuery, WM_SETFONT, (WPARAM)fonts->hMonoFont, TRUE);

    y += 70;

    /* Execute button */
    handles->hwndExecute = CreateWindowEx(0, "BUTTON", "Execute Query",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,
        x, y, 120, 24, hwnd, (HMENU)ID_BUTTON_EXECUTE, NULL, NULL);
    SendMessage(handles->hwndExecute, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);
    EnableWindow(handles->hwndExecute, FALSE);  /* Disabled until connected */

    /* Reset x, y for results section */
    x = 12;
    y = 286;

    /* Query Results Group Box */
    handles->hwndResultsGroup = CreateWindowEx(0, "BUTTON", "Query Results",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        x, y, 860, 280, hwnd, NULL, NULL, NULL);
    SendMessage(handles->hwndResultsGroup, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    x += 10;
    y += 20;

    /* ListView for query results (virtual list with LVS_OWNERDATA) */
    handles->hwndListView = CreateWindowEx(0, WC_LISTVIEW, "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP |
        LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_OWNERDATA,
        x, y, 840, 245, hwnd, (HMENU)ID_LISTVIEW_RESULTS, NULL, NULL);
    SendMessage(handles->hwndListView, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    /* Set ListView extended styles */
    ListView_SetExtendedListViewStyle(handles->hwndListView,
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    /* Buttons and row count below ListView */
    x = 22;
    y = 556;

    /* Clear Results button */
    handles->hwndClearResults = CreateWindowEx(0, "BUTTON", "Clear Results",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        x, y, 100, 24, hwnd, (HMENU)ID_BUTTON_CLEAR_RESULTS, NULL, NULL);
    SendMessage(handles->hwndClearResults, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    x += 110;

    /* Copy button */
    handles->hwndCopy = CreateWindowEx(0, "BUTTON", "Copy Selected",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        x, y, 110, 24, hwnd, (HMENU)ID_BUTTON_COPY, NULL, NULL);
    SendMessage(handles->hwndCopy, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    x += 120;

    /* Row count label */
    handles->hwndRowCount = CreateWindowEx(0, "STATIC", "Rows: 0",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x, y + 3, 200, 20, hwnd, (HMENU)ID_STATIC_ROW_COUNT, NULL, NULL);
    SendMessage(handles->hwndRowCount, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    /* Reset x, y for debug section */
    x = 12;
    y = 590;

    /* Debug Output Group Box */
    handles->hwndDebugGroup = CreateWindowEx(0, "BUTTON", "Debug Output",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        x, y, 860, 250, hwnd, NULL, NULL, NULL);
    SendMessage(handles->hwndDebugGroup, WM_SETFONT, (WPARAM)fonts->hFont, TRUE);

    x += 10;
    y += 20;

    /* Debug output window - use LISTBOX instead of EDIT for better rendering */
    handles->hwndDebug = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | LBS_NOINTEGRALHEIGHT | LBS_NOSEL,
        x, y, 840, 215, hwnd, (HMENU)ID_EDIT_DEBUG, NULL, NULL);
    SendMessage(handles->hwndDebug, WM_SETFONT, (WPARAM)fonts->hMonoFont, TRUE);

    /* Save handles globally */
    g_handles = *handles;
}

/* Resize controls when window is resized */
void Controls_Resize(HWND hwnd, const ControlHandles* handles)
{
    RECT rcClient;
    int margin = 12;
    int clientWidth, clientHeight;
    int groupWidth;
    int connectionHeight = 102;
    int connectButtonHeight = 24;
    int queryGroupHeight = 120;
    int resultsGroupHeight = 280;
    int debugGroupHeight;
    int availableHeight;

    /* Get client area size */
    GetClientRect(hwnd, &rcClient);
    clientWidth = rcClient.right - rcClient.left;
    clientHeight = rcClient.bottom - rcClient.top;

    /* Calculate widths */
    groupWidth = clientWidth - (2 * margin);

    /* Calculate available height for results and debug (split remaining space) */
    availableHeight = clientHeight - margin - connectionHeight - margin -
                      connectButtonHeight - margin - queryGroupHeight - margin;

    /* Split remaining space: 60% results, 40% debug (with minimums) */
    resultsGroupHeight = (int)(availableHeight * 0.6);
    debugGroupHeight = availableHeight - resultsGroupHeight - margin;

    /* Ensure minimum heights */
    if (resultsGroupHeight < 200)
        resultsGroupHeight = 200;
    if (debugGroupHeight < 150)
        debugGroupHeight = 150;

    /* Resize Connection Settings Group Box */
    MoveWindow(handles->hwndConnectionGroup,
        margin, margin,
        groupWidth, connectionHeight,
        TRUE);

    /* Connect button and Status stay below connection group */
    MoveWindow(handles->hwndConnect,
        margin, margin + connectionHeight + 10,
        100, connectButtonHeight,
        TRUE);

    MoveWindow(handles->hwndStatus,
        margin + 115, margin + connectionHeight + 10 + 3,
        300, 20,
        TRUE);

    /* Resize SQL Query Group Box */
    MoveWindow(handles->hwndQueryGroup,
        margin, margin + connectionHeight + 10 + connectButtonHeight + 10,
        groupWidth, queryGroupHeight,
        TRUE);

    /* Resize Query input (inside query group) */
    MoveWindow(handles->hwndQuery,
        margin + 10, margin + connectionHeight + 10 + connectButtonHeight + 10 + 20,
        groupWidth - 20, 65,
        TRUE);

    /* Resize Execute button (inside query group) */
    MoveWindow(handles->hwndExecute,
        margin + 10, margin + connectionHeight + 10 + connectButtonHeight + 10 + 20 + 70,
        120, 24,
        TRUE);

    /* Calculate Y position for results group */
    int resultsY = margin + connectionHeight + 10 + connectButtonHeight + 10 + queryGroupHeight + 10;

    /* Resize Query Results Group Box */
    MoveWindow(handles->hwndResultsGroup,
        margin, resultsY,
        groupWidth, resultsGroupHeight,
        TRUE);

    /* Resize ListView (inside results group) */
    MoveWindow(handles->hwndListView,
        margin + 10, resultsY + 20,
        groupWidth - 20, resultsGroupHeight - 65,
        TRUE);

    /* Position buttons and row count below ListView */
    int buttonY = resultsY + resultsGroupHeight - 35;
    MoveWindow(handles->hwndClearResults,
        margin + 10, buttonY,
        100, 24,
        TRUE);

    MoveWindow(handles->hwndCopy,
        margin + 120, buttonY,
        110, 24,
        TRUE);

    MoveWindow(handles->hwndRowCount,
        margin + 240, buttonY + 3,
        200, 20,
        TRUE);

    /* Calculate Y position for debug group */
    int debugY = resultsY + resultsGroupHeight + 10;

    /* Resize Debug Output Group Box */
    MoveWindow(handles->hwndDebugGroup,
        margin, debugY,
        groupWidth, debugGroupHeight,
        TRUE);

    /* Resize Debug output listbox (inside debug group) */
    MoveWindow(handles->hwndDebug,
        margin + 10, debugY + 20,
        groupWidth - 20, debugGroupHeight - 35,
        TRUE);
}

/* Update connection state and UI controls */
void Controls_UpdateConnectionState(const ControlHandles* handles, BOOL connected)
{
    if (connected)
    {
        /* Disable connection fields */
        EnableWindow(handles->hwndHost, FALSE);
        EnableWindow(handles->hwndPort, FALSE);
        EnableWindow(handles->hwndUser, FALSE);
        EnableWindow(handles->hwndPassword, FALSE);
        EnableWindow(handles->hwndDatabase, FALSE);

        /* Change button text to Disconnect */
        SetWindowText(handles->hwndConnect, "Disconnect");

        /* Enable execute button */
        EnableWindow(handles->hwndExecute, TRUE);

        /* Update status */
        Controls_UpdateStatus(handles->hwndStatus, "Connected");
    }
    else
    {
        /* Enable connection fields */
        EnableWindow(handles->hwndHost, TRUE);
        EnableWindow(handles->hwndPort, TRUE);
        EnableWindow(handles->hwndUser, TRUE);
        EnableWindow(handles->hwndPassword, TRUE);
        EnableWindow(handles->hwndDatabase, TRUE);

        /* Change button text to Connect */
        SetWindowText(handles->hwndConnect, "Connect");

        /* Disable execute button */
        EnableWindow(handles->hwndExecute, FALSE);

        /* Update status */
        Controls_UpdateStatus(handles->hwndStatus, "Disconnected");
    }
}

/* Update status label */
void Controls_UpdateStatus(HWND hwndStatus, const char* status)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Status: %s", status);
    SetWindowText(hwndStatus, buffer);
}

/* Update row count display */
void Controls_UpdateRowCount(HWND hwndRowCount, int count)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Rows: %d", count);
    SetWindowText(hwndRowCount, buffer);
}

/* Create application fonts */
void Controls_CreateFonts(FontHandles* fonts)
{
    /* Create fonts - Segoe UI (Vista+) or Tahoma (XP) */
    fonts->hFont = CreateFont(
        -11, 0, 0, 0,
        FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,          /* ClearType for smoother text */
        DEFAULT_PITCH | FF_SWISS,
        "Segoe UI"                   /* Modern font (Vista+) */
    );

    /* Create bold font for headers */
    fonts->hBoldFont = CreateFont(
        -11, 0, 0, 0,
        FW_BOLD,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        "Segoe UI"
    );

    /* Create monospace font for debug output */
    fonts->hMonoFont = CreateFont(
        -13, 0, 0, 0,
        FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        FIXED_PITCH | FF_MODERN,
        "Consolas"                   /* Modern monospace font */
    );
}

/* Cleanup application fonts */
void Controls_CleanupFonts(FontHandles* fonts)
{
    if (fonts->hFont) DeleteObject(fonts->hFont);
    if (fonts->hBoldFont) DeleteObject(fonts->hBoldFont);
    if (fonts->hMonoFont) DeleteObject(fonts->hMonoFont);
}

/* Get control handles for easy access */
ControlHandles* Controls_GetHandles(void)
{
    return &g_handles;
}
