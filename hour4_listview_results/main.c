/* pqview - PostgreSQL Query Client - Hour 4: ListView Results */

#include <windows.h>
#include <commctrl.h>  /* For InitCommonControlsEx */
#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

/* Window class name */
#define WINDOW_CLASS_NAME "pqview"

/* Window dimensions */
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 850

/* Control IDs */
#define ID_EDIT_HOST 101
#define ID_EDIT_PORT 102
#define ID_EDIT_USER 103
#define ID_EDIT_PASSWORD 104
#define ID_EDIT_DATABASE 105
#define ID_BUTTON_CONNECT 106
#define ID_EDIT_DEBUG 107
#define ID_STATIC_STATUS 108
#define ID_EDIT_QUERY 109
#define ID_BUTTON_EXECUTE 110
#define ID_LISTVIEW_RESULTS 111

/* Global variables */
HWND g_hwndHost, g_hwndPort, g_hwndUser, g_hwndPassword, g_hwndDatabase;
HWND g_hwndConnect, g_hwndDebug, g_hwndStatus;
HWND g_hwndQuery, g_hwndExecute;
HWND g_hwndListView;
HWND g_hwndConnectionGroup, g_hwndQueryGroup, g_hwndResultsGroup, g_hwndDebugGroup;
PGconn* g_conn = NULL;
PGresult* g_currentResult = NULL;  /* Cached result for virtual ListView */
HFONT g_hFont = NULL;
HFONT g_hBoldFont = NULL;
HFONT g_hMonoFont = NULL;

/* Forward declarations */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateControls(HWND hwnd);
void ResizeControls(HWND hwnd);
void DebugLog(const char* message);
void HandleConnect(HWND hwnd);
void HandleExecuteQuery(HWND hwnd);
void UpdateStatus(const char* status);

/* Entry point for Windows applications */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;
    INITCOMMONCONTROLSEX icex;

    /* Initialize common controls (required for XP themes) */
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    /* Initialize WNDCLASSEX structure */
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    /* Register the window class */
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    /* Create the window */
    hwnd = CreateWindowEx(
        WS_EX_WINDOWEDGE,
        WINDOW_CLASS_NAME,
        "pqview - PostgreSQL Query Client",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    /* Show the window */
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* Message loop */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

/* Window procedure */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            /* Create fonts - Segoe UI (Vista+) or Tahoma (XP) */
            g_hFont = CreateFont(
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
            g_hBoldFont = CreateFont(
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
            g_hMonoFont = CreateFont(
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

            CreateControls(hwnd);
            DebugLog("Application started");
            UpdateStatus("Disconnected");
            return 0;
        }

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == ID_BUTTON_CONNECT)
            {
                HandleConnect(hwnd);
            }
            else if (LOWORD(wParam) == ID_BUTTON_EXECUTE)
            {
                HandleExecuteQuery(hwnd);
            }
            return 0;
        }

        case WM_SIZE:
        {
            /* Resize controls when window is resized */
            ResizeControls(hwnd);
            return 0;
        }

        case WM_GETMINMAXINFO:
        {
            /* Set minimum window size */
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            mmi->ptMinTrackSize.x = WINDOW_WIDTH;
            mmi->ptMinTrackSize.y = WINDOW_HEIGHT;
            return 0;
        }

        case WM_NOTIFY:
        {
            LPNMHDR nmhdr = (LPNMHDR)lParam;
            if (nmhdr->idFrom == ID_LISTVIEW_RESULTS && nmhdr->code == LVN_GETDISPINFO)
            {
                NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
                static char buffer[256];

                if (g_currentResult != NULL && plvdi->item.iItem >= 0 && plvdi->item.iItem < PQntuples(g_currentResult))
                {
                    int row = plvdi->item.iItem;
                    int col = plvdi->item.iSubItem;

                    if (PQgetisnull(g_currentResult, row, col))
                    {
                        snprintf(buffer, sizeof(buffer), "(NULL)");
                    }
                    else
                    {
                        const char* value = PQgetvalue(g_currentResult, row, col);
                        snprintf(buffer, sizeof(buffer), "%s", value);
                    }

                    plvdi->item.pszText = buffer;
                }
            }
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        {
            /* Make static controls use dialog background color */
            HDC hdcStatic = (HDC)wParam;
            SetBkMode(hdcStatic, TRANSPARENT);
            return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
        }

        case WM_DESTROY:
        {
            /* Cleanup fonts */
            if (g_hFont) DeleteObject(g_hFont);
            if (g_hBoldFont) DeleteObject(g_hBoldFont);
            if (g_hMonoFont) DeleteObject(g_hMonoFont);

            /* Cleanup cached result */
            if (g_currentResult != NULL)
            {
                PQclear(g_currentResult);
                g_currentResult = NULL;
            }

            /* Close database connection if open */
            if (g_conn != NULL)
            {
                PQfinish(g_conn);
                g_conn = NULL;
            }
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

/* Create all UI controls */
void CreateControls(HWND hwnd)
{
    HWND hwndControl;
    int x = 12, y = 12;
    int labelWidth = 70;
    int fieldWidth = 160;
    int spacing = 26;

    /* Connection Settings Group Box */
    g_hwndConnectionGroup = CreateWindowEx(0, "BUTTON", "Connection Settings",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        x, y, 860, 102, hwnd, NULL, NULL, NULL);
    SendMessage(g_hwndConnectionGroup, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    x += 10;
    y += 18;

    /* Host */
    hwndControl = CreateWindowEx(0, "STATIC", "Host:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x, y, labelWidth, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    g_hwndHost = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "localhost",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 21, hwnd, (HMENU)ID_EDIT_HOST, NULL, NULL);
    SendMessage(g_hwndHost, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    /* Port */
    hwndControl = CreateWindowEx(0, "STATIC", "Port:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x + labelWidth + fieldWidth + 15, y, 40, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    g_hwndPort = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "5432",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth + fieldWidth + 55, y, 70, 21, hwnd, (HMENU)ID_EDIT_PORT, NULL, NULL);
    SendMessage(g_hwndPort, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    y += spacing;

    /* User */
    hwndControl = CreateWindowEx(0, "STATIC", "User:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x, y, labelWidth, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    g_hwndUser = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "postgres",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 21, hwnd, (HMENU)ID_EDIT_USER, NULL, NULL);
    SendMessage(g_hwndUser, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    /* Password */
    hwndControl = CreateWindowEx(0, "STATIC", "Password:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x + labelWidth + fieldWidth + 15, y, 60, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    g_hwndPassword = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "postgres",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_PASSWORD | ES_AUTOHSCROLL,
        x + labelWidth + fieldWidth + 75, y, 160, 21, hwnd, (HMENU)ID_EDIT_PASSWORD, NULL, NULL);
    SendMessage(g_hwndPassword, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    y += spacing;

    /* Database */
    hwndControl = CreateWindowEx(0, "STATIC", "Database:",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x, y, labelWidth, 20, hwnd, NULL, NULL, NULL);
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    g_hwndDatabase = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "stream_demo",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 21, hwnd, (HMENU)ID_EDIT_DATABASE, NULL, NULL);
    SendMessage(g_hwndDatabase, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    /* Reset x, y for controls below group box */
    x = 12;
    y = 122;

    /* Connect button */
    g_hwndConnect = CreateWindowEx(0, "BUTTON", "Connect",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,
        x, y, 100, 24, hwnd, (HMENU)ID_BUTTON_CONNECT, NULL, NULL);
    SendMessage(g_hwndConnect, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    /* Status */
    g_hwndStatus = CreateWindowEx(0, "STATIC", "Status: Disconnected",
        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
        x + 115, y + 3, 300, 20, hwnd, (HMENU)ID_STATIC_STATUS, NULL, NULL);
    SendMessage(g_hwndStatus, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    /* Reset x, y for query section */
    x = 12;
    y = 156;

    /* SQL Query Group Box */
    g_hwndQueryGroup = CreateWindowEx(0, "BUTTON", "SQL Query",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        x, y, 860, 120, hwnd, NULL, NULL, NULL);
    SendMessage(g_hwndQueryGroup, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    x += 10;
    y += 20;

    /* Query input - multi-line EDIT control */
    g_hwndQuery = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "SELECT * FROM customers LIMIT 10;",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL,
        x, y, 840, 65, hwnd, (HMENU)ID_EDIT_QUERY, NULL, NULL);
    SendMessage(g_hwndQuery, WM_SETFONT, (WPARAM)g_hMonoFont, TRUE);

    y += 70;

    /* Execute button */
    g_hwndExecute = CreateWindowEx(0, "BUTTON", "Execute Query",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,
        x, y, 120, 24, hwnd, (HMENU)ID_BUTTON_EXECUTE, NULL, NULL);
    SendMessage(g_hwndExecute, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    /* Reset x, y for results section */
    x = 12;
    y = 286;

    /* Query Results Group Box */
    g_hwndResultsGroup = CreateWindowEx(0, "BUTTON", "Query Results",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        x, y, 860, 280, hwnd, NULL, NULL, NULL);
    SendMessage(g_hwndResultsGroup, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    x += 10;
    y += 20;

    /* ListView for query results (virtual list with LVS_OWNERDATA) */
    g_hwndListView = CreateWindowEx(0, WC_LISTVIEW, "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP |
        LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_OWNERDATA,
        x, y, 840, 245, hwnd, (HMENU)ID_LISTVIEW_RESULTS, NULL, NULL);
    SendMessage(g_hwndListView, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    /* Set ListView extended styles */
    ListView_SetExtendedListViewStyle(g_hwndListView,
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    /* Reset x, y for debug section */
    x = 12;
    y = 576;

    /* Debug Output Group Box */
    g_hwndDebugGroup = CreateWindowEx(0, "BUTTON", "Debug Output",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        x, y, 860, 250, hwnd, NULL, NULL, NULL);
    SendMessage(g_hwndDebugGroup, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    x += 10;
    y += 20;

    /* Debug output window - use LISTBOX instead of EDIT for better rendering */
    g_hwndDebug = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | LBS_NOINTEGRALHEIGHT | LBS_NOSEL,
        x, y, 840, 215, hwnd, (HMENU)ID_EDIT_DEBUG, NULL, NULL);
    SendMessage(g_hwndDebug, WM_SETFONT, (WPARAM)g_hMonoFont, TRUE);
}

/* Append message to debug window with timestamp */
void DebugLog(const char* message)
{
    char buffer[1024];
    char timestamp[32];
    time_t now;
    struct tm* timeinfo;
    int index;

    /* Get current time */
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);

    /* Format message with timestamp (no \r\n needed for listbox) */
    snprintf(buffer, sizeof(buffer), "[%s] %s", timestamp, message);

    /* Add string to listbox */
    index = SendMessage(g_hwndDebug, LB_ADDSTRING, 0, (LPARAM)buffer);

    /* Scroll to bottom by setting top index to last item */
    SendMessage(g_hwndDebug, LB_SETTOPINDEX, index, 0);
}

/* Update status label */
void UpdateStatus(const char* status)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Status: %s", status);
    SetWindowText(g_hwndStatus, buffer);
}

/* Handle Connect button click */
void HandleConnect(HWND hwnd)
{
    char host[256], port[16], user[256], password[256], database[256];
    char conninfo[1024];
    char message[512];

    /* Get connection parameters from controls */
    GetWindowText(g_hwndHost, host, sizeof(host));
    GetWindowText(g_hwndPort, port, sizeof(port));
    GetWindowText(g_hwndUser, user, sizeof(user));
    GetWindowText(g_hwndPassword, password, sizeof(password));
    GetWindowText(g_hwndDatabase, database, sizeof(database));

    /* Close existing connection if any */
    if (g_conn != NULL)
    {
        DebugLog("Closing existing connection");
        PQfinish(g_conn);
        g_conn = NULL;
    }

    /* Build connection string */
    snprintf(conninfo, sizeof(conninfo),
        "host=%s port=%s dbname=%s user=%s password=%s",
        host, port, database, user, password);

    /* Log connection attempt */
    snprintf(message, sizeof(message), "Connecting to %s:%s as %s...", host, port, user);
    DebugLog(message);
    UpdateStatus("Connecting...");

    /* Attempt connection */
    g_conn = PQconnectdb(conninfo);

    /* Check connection status */
    if (PQstatus(g_conn) != CONNECTION_OK)
    {
        /* Connection failed */
        snprintf(message, sizeof(message), "Connection FAILED: %s", PQerrorMessage(g_conn));
        DebugLog(message);
        UpdateStatus("Connection Failed");

        PQfinish(g_conn);
        g_conn = NULL;
        return;
    }

    /* Connection successful */
    snprintf(message, sizeof(message), "Connected to database '%s' successfully!", database);
    DebugLog(message);
    UpdateStatus("Connected");

    /* Get and display server version */
    {
        PGresult* res = PQexec(g_conn, "SELECT version();");
        if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0)
        {
            const char* version = PQgetvalue(res, 0, 0);
            snprintf(message, sizeof(message), "Server: %s", version);
            DebugLog(message);
        }
        PQclear(res);
    }
}

/* Handle Execute Query button click */
void HandleExecuteQuery(HWND hwnd)
{
    char query[8192];
    char message[512];
    PGresult* res;
    int nRows, nCols;
    int i, j;
    DWORD startTime, endTime;

    /* Check if connected */
    if (g_conn == NULL || PQstatus(g_conn) != CONNECTION_OK)
    {
        DebugLog("ERROR: Not connected to database");
        MessageBox(hwnd, "Please connect to database first", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    /* Get query text from control */
    GetWindowText(g_hwndQuery, query, sizeof(query));

    /* Check if query is empty */
    if (strlen(query) == 0)
    {
        DebugLog("ERROR: Query is empty");
        MessageBox(hwnd, "Please enter a SQL query", "Error", MB_OK | MB_ICONWARNING);
        return;
    }

    /* Log query execution */
    DebugLog("Executing query...");
    snprintf(message, sizeof(message), "Query: %s", query);
    DebugLog(message);

    /* Execute query and measure time */
    startTime = GetTickCount();
    res = PQexec(g_conn, query);
    endTime = GetTickCount();

    /* Check result status */
    if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        /* Query failed */
        snprintf(message, sizeof(message), "ERROR: %s", PQerrorMessage(g_conn));
        DebugLog(message);
        MessageBox(hwnd, PQerrorMessage(g_conn), "Query Error", MB_OK | MB_ICONERROR);
        PQclear(res);
        return;
    }

    /* Get result dimensions */
    nRows = PQntuples(res);
    nCols = PQnfields(res);

    /* Log execution info */
    snprintf(message, sizeof(message), "Query OK: %d rows, %d columns, %lu ms",
        nRows, nCols, (unsigned long)(endTime - startTime));
    DebugLog(message);

    /* Clear previous cached result */
    if (g_currentResult != NULL)
    {
        PQclear(g_currentResult);
    }

    /* Store result for virtual list */
    g_currentResult = res;

    /* Clear ListView */
    ListView_DeleteAllItems(g_hwndListView);

    /* Remove all columns */
    while (ListView_DeleteColumn(g_hwndListView, 0))
        ;

    /* Add columns to ListView */
    LVCOLUMN lvc;
    ZeroMemory(&lvc, sizeof(LVCOLUMN));
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = 120;  /* Default column width */

    for (j = 0; j < nCols; j++)
    {
        const char* fieldName = PQfname(res, j);
        lvc.pszText = (LPSTR)fieldName;
        ListView_InsertColumn(g_hwndListView, j, &lvc);
    }

    /* Set item count for virtual list */
    ListView_SetItemCountEx(g_hwndListView, nRows, LVSICF_NOINVALIDATEALL);

    /* Auto-size columns to fit header */
    for (j = 0; j < nCols; j++)
    {
        ListView_SetColumnWidth(g_hwndListView, j, LVSCW_AUTOSIZE_USEHEADER);
    }

    /* Log completion */
    snprintf(message, sizeof(message), "Displayed %d rows in ListView (virtual mode)", nRows);
    DebugLog(message);
}

/* Resize controls when window is resized */
void ResizeControls(HWND hwnd)
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
    MoveWindow(g_hwndConnectionGroup,
        margin, margin,
        groupWidth, connectionHeight,
        TRUE);

    /* Connect button and Status stay below connection group */
    MoveWindow(g_hwndConnect,
        margin, margin + connectionHeight + 10,
        100, connectButtonHeight,
        TRUE);

    MoveWindow(g_hwndStatus,
        margin + 115, margin + connectionHeight + 10 + 3,
        300, 20,
        TRUE);

    /* Resize SQL Query Group Box */
    MoveWindow(g_hwndQueryGroup,
        margin, margin + connectionHeight + 10 + connectButtonHeight + 10,
        groupWidth, queryGroupHeight,
        TRUE);

    /* Resize Query input (inside query group) */
    MoveWindow(g_hwndQuery,
        margin + 10, margin + connectionHeight + 10 + connectButtonHeight + 10 + 20,
        groupWidth - 20, 65,
        TRUE);

    /* Resize Execute button (inside query group) */
    MoveWindow(g_hwndExecute,
        margin + 10, margin + connectionHeight + 10 + connectButtonHeight + 10 + 20 + 70,
        120, 24,
        TRUE);

    /* Calculate Y position for results group */
    int resultsY = margin + connectionHeight + 10 + connectButtonHeight + 10 + queryGroupHeight + 10;

    /* Resize Query Results Group Box */
    MoveWindow(g_hwndResultsGroup,
        margin, resultsY,
        groupWidth, resultsGroupHeight,
        TRUE);

    /* Resize ListView (inside results group) */
    MoveWindow(g_hwndListView,
        margin + 10, resultsY + 20,
        groupWidth - 20, resultsGroupHeight - 35,
        TRUE);

    /* Calculate Y position for debug group */
    int debugY = resultsY + resultsGroupHeight + 10;

    /* Resize Debug Output Group Box */
    MoveWindow(g_hwndDebugGroup,
        margin, debugY,
        groupWidth, debugGroupHeight,
        TRUE);

    /* Resize Debug output listbox (inside debug group) */
    MoveWindow(g_hwndDebug,
        margin + 10, debugY + 20,
        groupWidth - 20, debugGroupHeight - 35,
        TRUE);
}
