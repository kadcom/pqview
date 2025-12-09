cmake_minimum_required(VERSION 3.11)

project(pqview VERSION 1.0.0 LANGUAGES C RC)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set PostgreSQL paths manually using PostgreSQL_ROOT
# For PostgreSQL 9.3: cmake .. -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/9.3"
set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/9.3" CACHE PATH "PostgreSQL installation directory")

# Set include and library paths manually
set(PostgreSQL_INCLUDE_DIRS "${PostgreSQL_ROOT}/include")
set(PostgreSQL_LIBRARY_DIRS "${PostgreSQL_ROOT}/lib")
set(PostgreSQL_BIN_DIR "${PostgreSQL_ROOT}/bin")

# Find the libpq library
find_library(PostgreSQL_LIBRARIES
    NAMES pq libpq
    PATHS "${PostgreSQL_LIBRARY_DIRS}"
    NO_DEFAULT_PATH
)

if(NOT PostgreSQL_LIBRARIES)
    message(FATAL_ERROR "PostgreSQL library not found in ${PostgreSQL_LIBRARY_DIRS}")
endif()

# Print PostgreSQL info for debugging
message(STATUS "PostgreSQL root: ${PostgreSQL_ROOT}")
message(STATUS "PostgreSQL include dir: ${PostgreSQL_INCLUDE_DIRS}")
message(STATUS "PostgreSQL library dir: ${PostgreSQL_LIBRARY_DIRS}")
message(STATUS "PostgreSQL bin dir: ${PostgreSQL_BIN_DIR}")
message(STATUS "PostgreSQL library: ${PostgreSQL_LIBRARIES}")

# Source files
set(SOURCES
    main.c
    debuglog.c
    database.c
    controls.c
    clipboard.c
    app.rc
)

# Header files (for IDE organization)
set(HEADERS
    compat.h
    constants.h
    debuglog.h
    database.h
    controls.h
    clipboard.h
)

# Create executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PostgreSQL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link directories (using link_directories for CMake 3.11 compatibility)
link_directories(${PostgreSQL_LIBRARY_DIRS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PostgreSQL_LIBRARIES}
    # Windows libraries
    user32      # Windows API core
    gdi32       # Graphics Device Interface
    comctl32    # Common Controls (ListView, etc.)
    ws2_32      # Winsock (needed by libpq for network)
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)

# MSVC specific settings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)

    # Embed manifest for Common Controls v6 (XP+ themes)
    # Using set_target_properties for CMake 3.11 compatibility (no target_link_options)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/MANIFESTDEPENDENCY:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\""
    )
endif()

# List of required PostgreSQL 9.3 DLLs (OpenSSL 1.0.x legacy naming)
# Note: DLL names differ from PostgreSQL 17:
#   - ssleay32.dll instead of libssl-3-x64.dll
#   - libeay32.dll instead of libcrypto-3-x64.dll
#   - libintl-8.dll instead of libintl-9.dll
set(PGSQL_DLLS
    libpq.dll
    ssleay32.dll
    libeay32.dll
    libintl-8.dll
    libiconv-2.dll
)

# Post-build: Copy PostgreSQL DLLs to executable directory (with existence check)
foreach(dll ${PGSQL_DLLS})
    set(dll_path "${PostgreSQL_BIN_DIR}/${dll}")
    if(EXISTS "${dll_path}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${dll_path}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Copying ${dll} to output directory"
        )
    else()
        message(WARNING "DLL not found: ${dll_path} - you may need to copy it manually")
    endif()
endforeach()

# Post-build message
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "PostgreSQL 9.3 DLLs copied to output directory"
)
