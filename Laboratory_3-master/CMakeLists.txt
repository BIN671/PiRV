cmake_minimum_required(VERSION 3.20)
project(Laboratory3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(server
        main.cpp
        server.cpp
        server.hpp
)
target_link_libraries(server PRIVATE Boost::system pthread)

add_executable(Laboratory3
        main.cpp
        server.cpp
        server.hpp
)
target_link_libraries(Laboratory3 PRIVATE Boost::system pthread)

add_executable(client
        client.cpp
        client.hpp
)
target_link_libraries(client PRIVATE Boost::system pthread)
