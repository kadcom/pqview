/* controls.h - UI control creation and management */

#ifndef CONTROLS_H
#define CONTROLS_H

#include <windows.h>

/* Control handles structure */
typedef struct {
    HWND hwndHost;
    HWND hwndPort;
    HWND hwndUser;
    HWND hwndPassword;
    HWND hwndDatabase;
    HWND hwndConnect;
    HWND hwndDebug;
    HWND hwndStatus;
    HWND hwndQuery;
    HWND hwndExecute;
    HWND hwndListView;
    HWND hwndClearResults;
    HWND hwndCopy;
    HWND hwndRowCount;
    HWND hwndConnectionGroup;
    HWND hwndQueryGroup;
    HWND hwndResultsGroup;
    HWND hwndDebugGroup;
} ControlHandles;

/* Font handles structure */
typedef struct {
    HFONT hFont;
    HFONT hBoldFont;
    HFONT hMonoFont;
} FontHandles;

/* Create all UI controls */
void Controls_Create(HWND hwnd, ControlHandles* handles, const FontHandles* fonts);

/* Resize controls when window is resized */
void Controls_Resize(HWND hwnd, const ControlHandles* handles);

/* Update connection state and UI controls */
void Controls_UpdateConnectionState(const ControlHandles* handles, BOOL connected);

/* Update status label */
void Controls_UpdateStatus(HWND hwndStatus, const char* status);

/* Update row count display */
void Controls_UpdateRowCount(HWND hwndRowCount, int count);

/* Create application fonts */
void Controls_CreateFonts(FontHandles* fonts);

/* Cleanup application fonts */
void Controls_CleanupFonts(FontHandles* fonts);

/* Get control handles for easy access */
ControlHandles* Controls_GetHandles(void);

#endif /* CONTROLS_H */
