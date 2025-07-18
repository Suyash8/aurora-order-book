add_library(aurora_interface INTERFACE)
target_include_directories(aurora_interface INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
    "${nlohmann_json_SOURCE_DIR}"
)

target_link_libraries(aurora_interface INTERFACE
    Boost::system
    Boost::thread
    PkgConfig::GPERFTOOLS
)

add_subdirectory(core)
add_subdirectory(network)
add_subdirectory(processing)
# add_subdirectory(storage)
# add_subdirectory(utils)
