/* pqview - PostgreSQL Query Client - Hour 2: Connection UI + Debug Window */

#include <windows.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Window class name */
#define WINDOW_CLASS_NAME "pqview"

/* Window dimensions */
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 700

/* Control IDs */
#define ID_EDIT_HOST 101
#define ID_EDIT_PORT 102
#define ID_EDIT_USER 103
#define ID_EDIT_PASSWORD 104
#define ID_EDIT_DATABASE 105
#define ID_BUTTON_CONNECT 106
#define ID_EDIT_DEBUG 107
#define ID_STATIC_STATUS 108

/* Global variables */
HWND g_hwndHost, g_hwndPort, g_hwndUser, g_hwndPassword, g_hwndDatabase;
HWND g_hwndConnect, g_hwndDebug, g_hwndStatus;
PGconn* g_conn = NULL;

/* Forward declarations */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateControls(HWND hwnd);
void DebugLog(const char* message);
void HandleConnect(HWND hwnd);
void UpdateStatus(const char* status);

/* Entry point for Windows applications */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

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
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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
        0,
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
            return 0;
        }

        case WM_DESTROY:
        {
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
    int x = 10, y = 10;
    int labelWidth = 80;
    int fieldWidth = 150;
    int spacing = 30;

    /* Connection Settings Header */
    CreateWindowEx(0, "STATIC", "Connection Settings:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x, y, 200, 20, hwnd, NULL, NULL, NULL);
    y += 25;

    /* Host */
    CreateWindowEx(0, "STATIC", "Host:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x, y + 3, labelWidth, 20, hwnd, NULL, NULL, NULL);
    g_hwndHost = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "localhost",
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 22, hwnd, (HMENU)ID_EDIT_HOST, NULL, NULL);

    /* Port */
    CreateWindowEx(0, "STATIC", "Port:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x + labelWidth + fieldWidth + 20, y + 3, 50, 20, hwnd, NULL, NULL, NULL);
    g_hwndPort = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "5432",
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth + fieldWidth + 70, y, 80, 22, hwnd, (HMENU)ID_EDIT_PORT, NULL, NULL);
    y += spacing;

    /* User */
    CreateWindowEx(0, "STATIC", "User:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x, y + 3, labelWidth, 20, hwnd, NULL, NULL, NULL);
    g_hwndUser = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "postgres",
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 22, hwnd, (HMENU)ID_EDIT_USER, NULL, NULL);
    y += spacing;

    /* Password */
    CreateWindowEx(0, "STATIC", "Password:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x, y + 3, labelWidth, 20, hwnd, NULL, NULL, NULL);
    g_hwndPassword = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "postgres",
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_PASSWORD | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 22, hwnd, (HMENU)ID_EDIT_PASSWORD, NULL, NULL);
    y += spacing;

    /* Database */
    CreateWindowEx(0, "STATIC", "Database:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x, y + 3, labelWidth, 20, hwnd, NULL, NULL, NULL);
    g_hwndDatabase = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "stream_demo",
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        x + labelWidth, y, fieldWidth, 22, hwnd, (HMENU)ID_EDIT_DATABASE, NULL, NULL);
    y += spacing;

    /* Connect button */
    g_hwndConnect = CreateWindowEx(0, "BUTTON", "Connect",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x + labelWidth, y, 100, 28, hwnd, (HMENU)ID_BUTTON_CONNECT, NULL, NULL);

    /* Status */
    g_hwndStatus = CreateWindowEx(0, "STATIC", "Status: Disconnected",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x + labelWidth + 120, y + 5, 300, 20, hwnd, (HMENU)ID_STATIC_STATUS, NULL, NULL);
    y += 40;

    /* Separator line */
    CreateWindowEx(0, "STATIC", "",
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ,
        x, y, 860, 2, hwnd, NULL, NULL, NULL);
    y += 10;

    /* Debug Output Header */
    CreateWindowEx(0, "STATIC", "Debug Output:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x, y, 200, 20, hwnd, NULL, NULL, NULL);
    y += 25;

    /* Debug output window */
    g_hwndDebug = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        x, y, 860, 420, hwnd, (HMENU)ID_EDIT_DEBUG, NULL, NULL);
}

/* Append message to debug window with timestamp */
void DebugLog(const char* message)
{
    char buffer[1024];
    char timestamp[32];
    time_t now;
    struct tm* timeinfo;
    int len;

    /* Get current time */
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);

    /* Format message with timestamp */
    snprintf(buffer, sizeof(buffer), "[%s] %s\r\n", timestamp, message);

    /* Get current text length */
    len = GetWindowTextLength(g_hwndDebug);

    /* Move caret to end */
    SendMessage(g_hwndDebug, EM_SETSEL, len, len);

    /* Append new text */
    SendMessage(g_hwndDebug, EM_REPLACESEL, FALSE, (LPARAM)buffer);

    /* Scroll to bottom */
    SendMessage(g_hwndDebug, EM_SCROLLCARET, 0, 0);
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
