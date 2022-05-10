cmake_minimum_required(VERSION 3.0.0)
project(token_client_server VERSION 0.1.0)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)

set(Boost_USE_STATIC_LIBS ON)
#set(OPENSSL_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS program_options REQUIRED)
#find_package(OpenSSL REQUIRED)

include_directories(${BOOST_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

include_directories(common)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
