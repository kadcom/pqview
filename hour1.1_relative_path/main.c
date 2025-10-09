/* pqview - PostgreSQL Query Client - Hour 1.1: Relative Path Linking */
/* Basic window with PostgreSQL connection test */

#include <windows.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>

/* Window class name */
#define WINDOW_CLASS_NAME "pqview"

/* Window dimensions */
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/* Forward declarations */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TestPostgreSQLConnection(HWND hwnd);

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
        0,                              /* Optional window styles */
        WINDOW_CLASS_NAME,              /* Window class */
        "pqview - Hour 1.1",            /* Window title */
        WS_OVERLAPPEDWINDOW,            /* Window style */
        CW_USEDEFAULT, CW_USEDEFAULT,   /* Position (x, y) */
        WINDOW_WIDTH, WINDOW_HEIGHT,    /* Size (width, height) */
        NULL,                           /* Parent window */
        NULL,                           /* Menu */
        hInstance,                      /* Instance handle */
        NULL                            /* Additional application data */
    );

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    /* Show the window */
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* Test PostgreSQL connection on startup */
    TestPostgreSQLConnection(hwnd);

    /* Message loop */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

/* Window procedure - handles messages sent to the window */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            /* Window created - initialization code here */
            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc;
            RECT rect;
            RECT titleRect;
            RECT subtextRect;
            RECT instructionRect;
            const char* text = "pqview - Hour 1.1: Relative Path Linking";
            const char* subtext = "Window created successfully!";
            const char* instruction = "Close this window to exit.";

            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rect);

            /* Draw title */
            titleRect = rect;
            titleRect.top = 50;
            DrawText(hdc, text, -1, &titleRect, DT_CENTER | DT_TOP | DT_SINGLELINE);

            /* Draw subtext */
            subtextRect = rect;
            subtextRect.top = 80;
            DrawText(hdc, subtext, -1, &subtextRect, DT_CENTER | DT_TOP | DT_SINGLELINE);

            /* Draw instruction */
            instructionRect = rect;
            instructionRect.top = 110;
            DrawText(hdc, instruction, -1, &instructionRect, DT_CENTER | DT_TOP | DT_SINGLELINE);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
        {
            /* Window is being destroyed */
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

/* Test PostgreSQL connection */
void TestPostgreSQLConnection(HWND hwnd)
{
    PGconn* conn;
    PGresult* res;
    char errorMsg[1024];
    char successMsg[2048];
    const char* version;

    /* Connection string - modify these values for your setup */
    const char* conninfo = "host=localhost port=5432 dbname=stream_demo user=postgres password=postgres";

    /* Attempt to connect */
    conn = PQconnectdb(conninfo);

    /* Check connection status */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        /* Connection failed */
        snprintf(errorMsg, sizeof(errorMsg),
            "PostgreSQL Connection Failed!\n\n"
            "Error: %s\n\n"
            "Please check:\n"
            "1. PostgreSQL is running\n"
            "2. Database 'stream_demo' exists\n"
            "3. Connection parameters are correct\n"
            "4. Update connection string in main.c",
            PQerrorMessage(conn));

        MessageBox(hwnd, errorMsg, "Connection Error", MB_ICONERROR | MB_OK);
        PQfinish(conn);
        return;
    }

    /* Connection successful - get server version */
    res = PQexec(conn, "SELECT version();");

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0)
    {
        version = PQgetvalue(res, 0, 0);
        snprintf(successMsg, sizeof(successMsg),
            "PostgreSQL Connection Successful!\n\n"
            "Server Version:\n%s",
            version);

        MessageBox(hwnd, successMsg, "Connection Success", MB_ICONINFORMATION | MB_OK);
    }
    else
    {
        MessageBox(hwnd, "Connected, but couldn't get version", "Info", MB_ICONINFORMATION | MB_OK);
    }

    /* Cleanup */
    PQclear(res);
    PQfinish(conn);
}
