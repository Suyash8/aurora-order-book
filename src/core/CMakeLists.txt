add_library(aurora_core STATIC
    Configuration.cpp
)

target_link_libraries(aurora_core PRIVATE aurora_interface)

set_target_properties(aurora_core PROPERTIES FOLDER "Libraries")