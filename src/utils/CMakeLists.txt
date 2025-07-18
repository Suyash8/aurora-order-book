add_library(aurora_utils STATIC
    ""
)

target_link_libraries(aurora_utils PRIVATE
    aurora_interface
    aurora_core
)

set_target_properties(aurora_utils PROPERTIES FOLDER "Libraries")