cmake_minimum_required(VERSION 3.20)
project(grainfall LANGUAGES C CXX)

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
if(EMSCRIPTEN)
    set(PLATFORM Web CACHE STRING "" FORCE)
endif()
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

# --- sandbox executable ---
add_executable(grainfall
    src/main.cpp
    src/elements.cpp
    src/grid.cpp
    src/renderer.cpp
    src/ui.cpp
)
target_link_libraries(grainfall PRIVATE raylib)

if(MSVC)
    target_compile_options(grainfall PRIVATE /W4)
else()
    target_compile_options(grainfall PRIVATE -Wall -Wextra)
endif()

# --- web build (emcmake cmake -B build-web) ---
if(EMSCRIPTEN)
    set_target_properties(grainfall PROPERTIES SUFFIX ".html")
    target_link_options(grainfall PRIVATE
        -sUSE_GLFW=3
        -sALLOW_MEMORY_GROWTH=1
        --shell-file ${CMAKE_SOURCE_DIR}/web/shell.html
    )
    set_property(TARGET grainfall APPEND PROPERTY LINK_DEPENDS ${CMAKE_SOURCE_DIR}/web/shell.html)
endif()
