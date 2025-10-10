/* debuglog.c - Debug logging implementation */

#include "debuglog.h"
#include <stdio.h>
#include <time.h>

/* Global debug window handle */
static HWND g_hwndDebug = NULL;

/* Initialize debug log window handle */
void DebugLog_Init(HWND hwndDebug)
{
    g_hwndDebug = hwndDebug;
}

/* Append message to debug window with timestamp */
void DebugLog(const char* message)
{
    char buffer[1024];
    char timestamp[32];
    time_t now;
    struct tm* timeinfo;
    int index;

    if (g_hwndDebug == NULL)
        return;

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
