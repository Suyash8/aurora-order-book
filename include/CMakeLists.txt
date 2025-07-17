include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/core"
    "${PROJECT_SOURCE_DIR}/include/network"
    "${PROJECT_SOURCE_DIR}/include/processing"
    "${PROJECT_SOURCE_DIR}/include/storage"
    "${PROJECT_SOURCE_DIR}/include/utils"
)

add_library(aurora_include INTERFACE)

target_include_directories(aurora_include INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/core"
    "${PROJECT_SOURCE_DIR}/include/network"
    "${PROJECT_SOURCE_DIR}/include/processing"
    "${PROJECT_SOURCE_DIR}/include/storage"
    "${PROJECT_SOURCE_DIR}/include/utils"
)
