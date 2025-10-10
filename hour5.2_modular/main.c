/* main.c - pqview PostgreSQL Query Client - Hour 5.2: Modular */

#include <windows.h>
#include <commctrl.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "debuglog.h"
#include "database.h"
#include "controls.h"
#include "clipboard.h"

/* Global variables */
static ControlHandles g_handles = {0};
static FontHandles g_fonts = {0};
static HACCEL g_hAccel = NULL;

/* Forward declarations */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void HandleConnect(HWND hwnd);
void HandleDisconnect(HWND hwnd);
void HandleExecuteQuery(HWND hwnd);
void HandleClearResults(HWND hwnd);
void HandleCopyResults(HWND hwnd);
void CreateAccelerators(void);

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
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PQVIEW));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PQVIEW));

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

    /* Create keyboard accelerators */
    CreateAccelerators();

    /* Show the window */
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* Message loop with accelerator support */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(hwnd, g_hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
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
            /* Initialize database module */
            Database_Init();

            /* Create fonts */
            Controls_CreateFonts(&g_fonts);

            /* Create controls */
            Controls_Create(hwnd, &g_handles, &g_fonts);

            /* Initialize debug log */
            DebugLog_Init(g_handles.hwndDebug);

            /* Log startup */
            DebugLog("Application started");
            Controls_UpdateStatus(g_handles.hwndStatus, "Disconnected");
            return 0;
        }

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == ID_BUTTON_CONNECT)
            {
                if (Database_IsConnected())
                {
                    HandleDisconnect(hwnd);
                }
                else
                {
                    HandleConnect(hwnd);
                }
            }
            else if (LOWORD(wParam) == ID_BUTTON_EXECUTE || LOWORD(wParam) == IDM_EXECUTE_QUERY)
            {
                HandleExecuteQuery(hwnd);
            }
            else if (LOWORD(wParam) == ID_BUTTON_CLEAR_RESULTS || LOWORD(wParam) == IDM_CLEAR_RESULTS)
            {
                HandleClearResults(hwnd);
            }
            else if (LOWORD(wParam) == ID_BUTTON_COPY || LOWORD(wParam) == IDM_COPY_RESULTS)
            {
                HandleCopyResults(hwnd);
            }
            return 0;
        }

        case WM_SIZE:
        {
            /* Resize controls when window is resized */
            Controls_Resize(hwnd, &g_handles);
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
                PGresult* result = Database_GetCurrentResult();

                if (result != NULL && plvdi->item.iItem >= 0 && plvdi->item.iItem < PQntuples(result))
                {
                    int row = plvdi->item.iItem;
                    int col = plvdi->item.iSubItem;

                    if (PQgetisnull(result, row, col))
                    {
                        snprintf(buffer, sizeof(buffer), "(NULL)");
                    }
                    else
                    {
                        const char* value = PQgetvalue(result, row, col);
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

        case WM_CLOSE:
        {
            /* Cleanup before closing */
            Database_Cleanup();
            DestroyWindow(hwnd);
            return 0;
        }

        case WM_DESTROY:
        {
            /* Cleanup fonts */
            Controls_CleanupFonts(&g_fonts);

            /* Final database cleanup */
            Database_Cleanup();

            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

/* Handle Connect button click */
void HandleConnect(HWND hwnd)
{
    ConnectionParams params;

    /* Get connection parameters from controls */
    GetWindowText(g_handles.hwndHost, params.host, sizeof(params.host));
    GetWindowText(g_handles.hwndPort, params.port, sizeof(params.port));
    GetWindowText(g_handles.hwndUser, params.user, sizeof(params.user));
    GetWindowText(g_handles.hwndPassword, params.password, sizeof(params.password));
    GetWindowText(g_handles.hwndDatabase, params.database, sizeof(params.database));

    /* Update status */
    Controls_UpdateStatus(g_handles.hwndStatus, "Connecting...");

    /* Attempt connection */
    if (Database_Connect(&params))
    {
        Controls_UpdateConnectionState(&g_handles, TRUE);
    }
    else
    {
        Controls_UpdateConnectionState(&g_handles, FALSE);
        Controls_UpdateStatus(g_handles.hwndStatus, "Connection Failed");
        MessageBox(hwnd, "Failed to connect to database", "Connection Error", MB_OK | MB_ICONERROR);
    }
}

/* Handle Disconnect button click */
void HandleDisconnect(HWND hwnd)
{
    /* Disconnect from database */
    Database_Disconnect();

    /* Clear ListView */
    ListView_DeleteAllItems(g_handles.hwndListView);
    while (ListView_DeleteColumn(g_handles.hwndListView, 0))
        ;

    /* Update row count */
    Controls_UpdateRowCount(g_handles.hwndRowCount, 0);

    /* Update connection state */
    Controls_UpdateConnectionState(&g_handles, FALSE);
}

/* Handle Execute Query button click */
void HandleExecuteQuery(HWND hwnd)
{
    char query[8192];
    char message[512];
    PGresult* res;
    int nRows, nCols;
    int j;
    DWORD executionTime;

    /* Check if connected */
    if (!Database_CheckConnection())
    {
        MessageBox(hwnd, "Please connect to database first", "Error", MB_OK | MB_ICONERROR);
        Controls_UpdateConnectionState(&g_handles, FALSE);
        return;
    }

    /* Get query text from control */
    GetWindowText(g_handles.hwndQuery, query, sizeof(query));

    /* Check if query is empty */
    if (strlen(query) == 0)
    {
        DebugLog("ERROR: Query is empty");
        MessageBox(hwnd, "Please enter a SQL query", "Error", MB_OK | MB_ICONWARNING);
        return;
    }

    /* Execute query */
    res = Database_ExecuteQuery(query, &executionTime);
    if (res == NULL)
    {
        MessageBox(hwnd, "Query execution failed", "Query Error", MB_OK | MB_ICONERROR);
        return;
    }

    /* Get result dimensions */
    nRows = PQntuples(res);
    nCols = PQnfields(res);

    /* Store result for virtual list */
    Database_SetCurrentResult(res);

    /* Clear ListView */
    ListView_DeleteAllItems(g_handles.hwndListView);

    /* Remove all columns */
    while (ListView_DeleteColumn(g_handles.hwndListView, 0))
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
        ListView_InsertColumn(g_handles.hwndListView, j, &lvc);
    }

    /* Set item count for virtual list */
    ListView_SetItemCountEx(g_handles.hwndListView, nRows, LVSICF_NOINVALIDATEALL);

    /* Auto-size columns to fit header */
    for (j = 0; j < nCols; j++)
    {
        ListView_SetColumnWidth(g_handles.hwndListView, j, LVSCW_AUTOSIZE_USEHEADER);
    }

    /* Update row count display */
    Controls_UpdateRowCount(g_handles.hwndRowCount, nRows);

    /* Log completion */
    snprintf(message, sizeof(message), "Displayed %d rows in ListView (virtual mode)", nRows);
    DebugLog(message);
}

/* Handle Clear Results button */
void HandleClearResults(HWND hwnd)
{
    /* Clear ListView */
    ListView_DeleteAllItems(g_handles.hwndListView);

    /* Remove all columns */
    while (ListView_DeleteColumn(g_handles.hwndListView, 0))
        ;

    /* Clear cached result */
    Database_SetCurrentResult(NULL);

    /* Update row count */
    Controls_UpdateRowCount(g_handles.hwndRowCount, 0);

    DebugLog("Results cleared");
}

/* Handle Copy Results button */
void HandleCopyResults(HWND hwnd)
{
    Clipboard_CopySelectedRow(hwnd, g_handles.hwndListView, Database_GetCurrentResult());
}

/* Create keyboard accelerators */
void CreateAccelerators(void)
{
    ACCEL accel[3];

    /* F5 - Execute Query */
    accel[0].fVirt = FVIRTKEY;
    accel[0].key = VK_F5;
    accel[0].cmd = IDM_EXECUTE_QUERY;

    /* Ctrl+L - Clear Results */
    accel[1].fVirt = FVIRTKEY | FCONTROL;
    accel[1].key = 'L';
    accel[1].cmd = IDM_CLEAR_RESULTS;

    /* Ctrl+C - Copy (when ListView focused) */
    accel[2].fVirt = FVIRTKEY | FCONTROL;
    accel[2].key = 'C';
    accel[2].cmd = IDM_COPY_RESULTS;

    g_hAccel = CreateAcceleratorTable(accel, 3);
}
