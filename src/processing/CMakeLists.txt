add_library(aurora_processing STATIC
    MessageParser.cpp
)

target_link_libraries(aurora_processing PRIVATE
    aurora_interface
    aurora_core
)

set_target_properties(aurora_processing PROPERTIES FOLDER "Libraries")