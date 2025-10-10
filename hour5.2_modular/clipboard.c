/* clipboard.c - Clipboard copy operations implementation */

#include "clipboard.h"
#include "debuglog.h"
#include <commctrl.h>
#include <stdio.h>
#include <string.h>

/* Copy selected row from ListView to clipboard */
BOOL Clipboard_CopySelectedRow(HWND hwnd, HWND hwndListView, PGresult* result)
{
    int selectedIndex;
    int nCols;
    char buffer[8192];
    char cellText[256];
    int j;

    /* Check if there's a result */
    if (result == NULL)
    {
        DebugLog("No results to copy");
        return FALSE;
    }

    /* Get selected item */
    selectedIndex = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
    if (selectedIndex == -1)
    {
        DebugLog("No row selected");
        MessageBox(hwnd, "Please select a row to copy", "Copy", MB_OK | MB_ICONINFORMATION);
        return FALSE;
    }

    /* Get number of columns */
    nCols = PQnfields(result);

    /* Build tab-separated row */
    buffer[0] = '\0';
    for (j = 0; j < nCols; j++)
    {
        if (PQgetisnull(result, selectedIndex, j))
        {
            strncat(buffer, "(NULL)", sizeof(buffer) - strlen(buffer) - 1);
        }
        else
        {
            const char* value = PQgetvalue(result, selectedIndex, j);
            strncat(buffer, value, sizeof(buffer) - strlen(buffer) - 1);
        }

        if (j < nCols - 1)
        {
            strncat(buffer, "\t", sizeof(buffer) - strlen(buffer) - 1);
        }
    }

    /* Copy to clipboard */
    if (OpenClipboard(hwnd))
    {
        HGLOBAL hMem;
        char* pMem;
        size_t len = strlen(buffer) + 1;

        EmptyClipboard();

        hMem = GlobalAlloc(GMEM_MOVEABLE, len);
        if (hMem)
        {
            pMem = (char*)GlobalLock(hMem);
            if (pMem)
            {
                memcpy(pMem, buffer, len);
                GlobalUnlock(hMem);
                SetClipboardData(CF_TEXT, hMem);
            }
        }

        CloseClipboard();

        snprintf(cellText, sizeof(cellText), "Copied row %d to clipboard", selectedIndex + 1);
        DebugLog(cellText);
        return TRUE;
    }
    else
    {
        DebugLog("ERROR: Could not open clipboard");
        return FALSE;
    }
}
