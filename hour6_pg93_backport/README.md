# Hour 6: PostgreSQL 9.3 Backport

This version backports the modular PostgreSQL Query Client to work with PostgreSQL 9.3 on Windows XP.

## Changes from Hour 5.2

### CMakeLists.txt Changes

The only changes are in CMakeLists.txt for PostgreSQL 9.3 compatibility:

1. **Default PostgreSQL path**: Changed to `C:/Program Files/PostgreSQL/9.3`

2. **DLL names**: Updated for OpenSSL 1.0.x legacy naming:

| PostgreSQL 17 | PostgreSQL 9.3 |
|---------------|----------------|
| `libssl-3-x64.dll` | `ssleay32.dll` |
| `libcrypto-3-x64.dll` | `libeay32.dll` |
| `libintl-9.dll` | `libintl-8.dll` |

3. **DLL existence checking**: Added warnings for missing DLLs during CMake configure

### No C Code Changes

All libpq functions used are available in PostgreSQL 9.3:
- `PQconnectdb`, `PQstatus`, `PQfinish`, `PQerrorMessage`
- `PQexec`, `PQresultStatus`, `PQclear`
- `PQntuples`, `PQnfields`, `PQgetvalue`, `PQgetisnull`, `PQfname`

## Required DLLs

Ensure these DLLs exist in your PostgreSQL 9.3 `bin/` directory:
- `libpq.dll` - PostgreSQL client library
- `ssleay32.dll` - OpenSSL SSL library
- `libeay32.dll` - OpenSSL crypto library
- `libintl-8.dll` - GNU gettext (may be `libintl.dll`)
- `libiconv-2.dll` - Character encoding

## Building

```bash
cd hour6_pg93_backport
mkdir build
cd build
cmake .. -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/9.3"
cmake --build .
```

If CMake warns about missing DLLs, verify the names in your PostgreSQL installation and update CMakeLists.txt accordingly.

## Windows XP Notes

- Requires Visual C++ runtime (`msvcr100.dll` for PostgreSQL 9.3)
- Common Controls v6 manifest is embedded for XP themes
- 32-bit or 64-bit depending on your PostgreSQL installation

## Module Structure

Same as Hour 5.2:
- `constants.h` - Control IDs and constants
- `debuglog.c/h` - Timestamped logging
- `database.c/h` - PostgreSQL operations
- `controls.c/h` - UI control management
- `clipboard.c/h` - Clipboard operations
- `main.c` - Application entry point
