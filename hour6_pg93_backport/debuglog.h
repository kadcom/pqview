/* debuglog.h - Debug logging functionality */

#ifndef DEBUGLOG_H
#define DEBUGLOG_H

#include <windows.h>

/* Initialize debug log window handle */
void DebugLog_Init(HWND hwndDebug);

/* Append timestamped message to debug window */
void DebugLog(const char* message);

#endif /* DEBUGLOG_H */
