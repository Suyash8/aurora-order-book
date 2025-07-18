cmake_minimum_required(VERSION 3.16)
project(AuroraLOB VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# --- Dependency Management with FetchContent ---
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
    URL            https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp
    URL_HASH       SHA256=aaf127c04cb31c406e5b04a63f1ae89369fccde6d8fa7cdda1ed4f32dfc5de63
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.8.3
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable building Google Benchmark's tests")
FetchContent_MakeAvailable(googlebenchmark)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        0.8.0
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_LIST_DIR}/patches/yaml-cpp.CMakeLists.txt"
        "<SOURCE_DIR>/CMakeLists.txt"
)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable yaml-cpp tests")
FetchContent_MakeAvailable(yaml-cpp)

FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG        v3.1.1
)
FetchContent_MakeAvailable(cxxopts)

# --- System Dependency Checks ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPERFTOOLS REQUIRED IMPORTED_TARGET libprofiler)

find_package(Boost 1.74.0 REQUIRED COMPONENTS system thread)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost 1.74.0 or higher.")
endif()

add_subdirectory(src)

add_executable(aurora
    src/main.cpp
)

target_link_libraries(aurora PRIVATE
    aurora_interface
)

target_link_libraries(aurora PRIVATE
    aurora_core
    aurora_network
    aurora_processing
    # aurora_storage
    # aurora_utils
)

set_target_properties(aurora PROPERTIES FOLDER "Executables")

enable_testing()
add_subdirectory(tests)