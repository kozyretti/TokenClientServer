set(SOURCE_FILES main.cpp client.cpp client.h)

add_executable(client ${SOURCE_FILES})

target_link_libraries(client PRIVATE common)
