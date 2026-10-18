cmake_minimum_required(VERSION 3.20)
project(falling_sand LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to an optimized build; the simulation is noticeably slower in Debug.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# --- raylib (fetched at configure time, no system install needed) ---
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

# --- sandbox executable ---
add_executable(falling_sand
    src/main.cpp
    src/elements.cpp
    src/grid.cpp
    src/renderer.cpp
    src/ui.cpp
)
target_link_libraries(falling_sand PRIVATE raylib)

if(MSVC)
    target_compile_options(falling_sand PRIVATE /W4)
else()
    target_compile_options(falling_sand PRIVATE -Wall -Wextra)
endif()
