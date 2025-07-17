add_executable(run_tests
    DataTypesTests.cpp
)

target_link_libraries(run_tests PRIVATE
    aurora_core
    Catch2::Catch2WithMain
)

set_target_properties(run_tests PROPERTIES FOLDER "Tests")