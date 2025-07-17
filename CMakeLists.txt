cmake_minimum_required(VERSION 3.16)
project(AuroraLOB VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.6.0
)
set(CATCH_BUILD_TESTING OFF CACHE BOOL "Disable building Catch2's own self-tests")
FetchContent_MakeAvailable(catch2)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.8.3
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable building Google Benchmark's tests")
FetchContent_MakeAvailable(googlebenchmark)

# find_package(gperftools)
# if(NOT gperftools_FOUND)
#     message(FATAL_ERROR "gperftools not found. Please install it system-wide (e.g., 'sudo apt-get install libgoogle-perftools-dev').")
# endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPERFTOOLS REQUIRED IMPORTED_TARGET libprofiler)

find_package(Boost 1.74.0 REQUIRED COMPONENTS system thread)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost 1.74.0 or higher.")
endif()


enable_testing()

add_subdirectory(src)
add_subdirectory(tests)