add_executable(run_tests
    DataTypesTests.cpp
)

target_link_libraries(run_tests PRIVATE
    Catch2::Catch2WithMain
    aurora_interface
    aurora_core
)

include(CTest)
add_test(NAME UnitTests COMMAND run_tests)

set_target_properties(run_tests PROPERTIES FOLDER "Tests")
