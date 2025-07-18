add_library(aurora_network STATIC
    TCPServer.cpp
    Session.cpp
)

target_link_libraries(aurora_network PRIVATE
    aurora_interface
    aurora_core
)

set_target_properties(aurora_network PROPERTIES FOLDER "Libraries")