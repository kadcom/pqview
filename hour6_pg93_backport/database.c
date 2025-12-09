/* database.c - PostgreSQL database operations implementation */

#include "database.h"
#include "debuglog.h"
#include "compat.h"
#include <stdio.h>
#include <string.h>

/* Global database state */
static PGconn* g_conn = NULL;
static PGresult* g_currentResult = NULL;
static BOOL g_isConnected = FALSE;

/* Initialize database module */
void Database_Init(void)
{
    g_conn = NULL;
    g_currentResult = NULL;
    g_isConnected = FALSE;
}

/* Connect to PostgreSQL database */
BOOL Database_Connect(const ConnectionParams* params)
{
    char conninfo[1024];
    char message[512];

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
        params->host, params->port, params->database, params->user, params->password);

    /* Log connection attempt */
    snprintf(message, sizeof(message), "Connecting to %s:%s as %s...",
        params->host, params->port, params->user);
    DebugLog(message);

    /* Attempt connection */
    g_conn = PQconnectdb(conninfo);

    /* Check connection status */
    if (PQstatus(g_conn) != CONNECTION_OK)
    {
        /* Connection failed */
        snprintf(message, sizeof(message), "Connection FAILED: %s", PQerrorMessage(g_conn));
        DebugLog(message);

        PQfinish(g_conn);
        g_conn = NULL;
        g_isConnected = FALSE;
        return FALSE;
    }

    /* Connection successful */
    snprintf(message, sizeof(message), "Connected to database '%s' successfully!", params->database);
    DebugLog(message);
    g_isConnected = TRUE;

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

    return TRUE;
}

/* Disconnect from database */
void Database_Disconnect(void)
{
    /* Close connection */
    if (g_conn != NULL)
    {
        DebugLog("Disconnecting from database...");
        PQfinish(g_conn);
        g_conn = NULL;
    }

    /* Clear cached result */
    if (g_currentResult != NULL)
    {
        PQclear(g_currentResult);
        g_currentResult = NULL;
    }

    g_isConnected = FALSE;
    DebugLog("Disconnected from database");
}

/* Execute a SQL query and return result */
PGresult* Database_ExecuteQuery(const char* query, DWORD* executionTime)
{
    PGresult* res;
    char message[512];
    DWORD startTime, endTime;
    int nRows, nCols;  /* Moved here for C89 compatibility */

    if (g_conn == NULL)
    {
        DebugLog("ERROR: Not connected to database");
        return NULL;
    }

    /* Log query execution */
    DebugLog("Executing query...");
    snprintf(message, sizeof(message), "Query: %s", query);
    DebugLog(message);

    /* Execute query and measure time */
    startTime = GetTickCount();
    res = PQexec(g_conn, query);
    endTime = GetTickCount();

    if (executionTime)
        *executionTime = endTime - startTime;

    /* Check result status */
    if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        /* Query failed */
        snprintf(message, sizeof(message), "ERROR: %s", PQerrorMessage(g_conn));
        DebugLog(message);
        PQclear(res);
        return NULL;
    }

    /* Get result dimensions */
    nRows = PQntuples(res);
    nCols = PQnfields(res);

    /* Log execution info */
    snprintf(message, sizeof(message), "Query OK: %d rows, %d columns, %lu ms",
        nRows, nCols, (unsigned long)(executionTime ? *executionTime : 0));
    DebugLog(message);

    return res;
}

/* Check if connection is valid */
BOOL Database_CheckConnection(void)
{
    char message[512];

    /* Check if connection exists */
    if (g_conn == NULL)
    {
        DebugLog("ERROR: Not connected to database");
        g_isConnected = FALSE;
        return FALSE;
    }

    /* Check connection status */
    if (PQstatus(g_conn) != CONNECTION_OK)
    {
        snprintf(message, sizeof(message), "ERROR: Connection lost: %s", PQerrorMessage(g_conn));
        DebugLog(message);

        /* Cleanup disconnected connection */
        PQfinish(g_conn);
        g_conn = NULL;

        /* Clear cached result */
        if (g_currentResult != NULL)
        {
            PQclear(g_currentResult);
            g_currentResult = NULL;
        }

        g_isConnected = FALSE;
        return FALSE;
    }

    return TRUE;
}

/* Get current connection handle */
PGconn* Database_GetConnection(void)
{
    return g_conn;
}

/* Get current result (for virtual ListView) */
PGresult* Database_GetCurrentResult(void)
{
    return g_currentResult;
}

/* Set current result (transfers ownership) */
void Database_SetCurrentResult(PGresult* result)
{
    /* Clear previous cached result */
    if (g_currentResult != NULL)
    {
        PQclear(g_currentResult);
    }

    g_currentResult = result;
}

/* Cleanup all database resources */
void Database_Cleanup(void)
{
    /* Clear cached result */
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

    g_isConnected = FALSE;
}

/* Get connection status */
BOOL Database_IsConnected(void)
{
    return g_isConnected;
}
