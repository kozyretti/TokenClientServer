set(SOURCE_FILES main.cpp server.cpp server.h server_session.cpp server_session.h
    periodic_output.cpp periodic_output.h token_handlers_manager.hpp thread_safe_map.hpp
    delay_simulation.cpp delay_simulation.h)

add_executable(server ${SOURCE_FILES})

target_link_libraries(server PRIVATE common)
