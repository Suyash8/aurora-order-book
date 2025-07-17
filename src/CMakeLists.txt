add_subdirectory(core)

add_executable(aurora
    main.cpp
)

target_link_libraries(aurora PRIVATE aurora_core
)

target_include_directories(aurora PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)

install(TARGETS aurora
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)