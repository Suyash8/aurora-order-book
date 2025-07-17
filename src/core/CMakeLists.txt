add_library(aurora_core STATIC
    Configuration.cpp
)

target_include_directories(aurora_core PUBLIC
    "${PROJECT_SOURCE_DIR}/include/core"
)