add_executable(aurora
    main.cpp
)

target_include_directories(aurora PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)

install(TARGETS aurora
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)