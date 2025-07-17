cmake_minimum_required(VERSION 3.16)
project(AuroraLOB VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include(FetchContent)

FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.6.0
)

set(CATCH_BUILD_TESTING OFF CACHE BOOL "Disable building Catch2's own self-tests")
set(CATCH_BUILD_EXAMPLES OFF CACHE BOOL "Disable building Catch2's examples")
set(CATCH_BUILD_EXTRA_TESTS OFF CACHE BOOL "Disable building Catch2's extra tests")

FetchContent_MakeAvailable(catch2)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)