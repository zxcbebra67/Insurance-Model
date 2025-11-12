cmake_minimum_required(VERSION 3.10)
project(InsuranceServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(InsuranceServer
    server.cpp
    Insurance.cpp
)

if(WIN32)
    target_link_libraries(InsuranceServer ws2_32 wsock32)
else()
    target_link_libraries(InsuranceServer pthread)
endif()