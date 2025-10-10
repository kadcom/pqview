/* database.h - PostgreSQL database operations */

#ifndef DATABASE_H
#define DATABASE_H

#include <windows.h>
#include <libpq-fe.h>

/* Connection parameters structure */
typedef struct {
    char host[256];
    char port[16];
    char user[256];
    char password[256];
    char database[256];
} ConnectionParams;

/* Initialize database module */
void Database_Init(void);

/* Connect to PostgreSQL database */
BOOL Database_Connect(const ConnectionParams* params);

/* Disconnect from database */
void Database_Disconnect(void);

/* Execute a SQL query and return result */
PGresult* Database_ExecuteQuery(const char* query, DWORD* executionTime);

/* Check if connection is valid */
BOOL Database_CheckConnection(void);

/* Get current connection handle */
PGconn* Database_GetConnection(void);

/* Get current result (for virtual ListView) */
PGresult* Database_GetCurrentResult(void);

/* Set current result (transfers ownership) */
void Database_SetCurrentResult(PGresult* result);

/* Cleanup all database resources */
void Database_Cleanup(void);

/* Get connection status */
BOOL Database_IsConnected(void);

#endif /* DATABASE_H */
