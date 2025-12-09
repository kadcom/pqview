/* clipboard.h - Clipboard copy operations */

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <windows.h>
#include <libpq-fe.h>

/* Copy selected row from ListView to clipboard */
BOOL Clipboard_CopySelectedRow(HWND hwnd, HWND hwndListView, PGresult* result);

#endif /* CLIPBOARD_H */
