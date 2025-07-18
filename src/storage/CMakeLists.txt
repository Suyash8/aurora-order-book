add_library(aurora_storage STATIC
    ""
)

target_link_libraries(aurora_storage PRIVATE
    aurora_interface
    aurora_core
)

set_target_properties(aurora_storage PROPERTIES FOLDER "Libraries")