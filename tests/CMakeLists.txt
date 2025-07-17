add_executable(run_tests
    DataTypesTests.cpp
)

target_link_libraries(run_tests PRIVATE
    aurora_core
    Catch2::Catch2WithMain
)

target_include_directories(run_tests PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)