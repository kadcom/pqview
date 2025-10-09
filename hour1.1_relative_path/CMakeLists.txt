cmake_minimum_required(VERSION 3.20)

project(pqview VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set PostgreSQL paths manually using PostgreSQL_ROOT
# Can be set via: cmake .. -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/17"
set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/17" CACHE PATH "PostgreSQL installation directory")

# Set include and library paths manually
set(PostgreSQL_INCLUDE_DIRS "${PostgreSQL_ROOT}/include")
set(PostgreSQL_LIBRARY_DIRS "${PostgreSQL_ROOT}/lib")

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
message(STATUS "PostgreSQL library: ${PostgreSQL_LIBRARIES}")

# Source files
set(SOURCES
    main.c
)

# Create executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PostgreSQL_INCLUDE_DIRS}
)

# Link directories
target_link_directories(${PROJECT_NAME} PRIVATE
    ${PostgreSQL_LIBRARY_DIRS}
)

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
endif()

# Post-build message
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed successfully!"
)
