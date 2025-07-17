add_subdirectory(core)

target_link_libraries(aurora_core PUBLIC
    nlohmann_json::nlohmann_json
    Boost::system
    Boost::thread
    PkgConfig::GPERFTOOLS
)

add_executable(aurora
    main.cpp
)

target_include_directories(aurora PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(aurora PRIVATE
    aurora_core
)

set_target_properties(aurora_core PROPERTIES FOLDER "Source")
set_target_properties(aurora PROPERTIES FOLDER "Executables")