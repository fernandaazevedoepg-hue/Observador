cmake_minimum_required(VERSION 3.16)
project(Observador LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(Observador ${SRC_FILES})

target_include_directories(Observador PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Prefer vendored static raylib if present
if(EXISTS "${CMAKE_SOURCE_DIR}/lib/libraylib.a")
    target_link_libraries(Observador PRIVATE "${CMAKE_SOURCE_DIR}/lib/libraylib.a")
    # raylib deps on Linux desktop
    if(UNIX AND NOT APPLE)
        target_link_libraries(Observador PRIVATE m dl pthread X11)
    endif()
else()
    find_package(raylib 4.0 QUIET)
    if(raylib_FOUND)
        target_link_libraries(Observador PRIVATE raylib)
    else()
        message(FATAL_ERROR "raylib not found. Install raylib dev package or add lib/libraylib.a")
    endif()
endif()

# Assets: run from project root so relative paths like assets/... work
set_target_properties(Observador PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
