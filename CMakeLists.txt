# Порхай как бабочка, жаль что петух
cmake_minimum_required(VERSION 3.16)
project(InsuranceSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 🔧 Обязательно включить, чтобы Qt генерировал moc_mainwindow.cpp
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(InsuranceSim
        main.cpp
        mainwindow.h
        mainwindow.cpp
        policy.h
        policy.cpp
        company.h
        company.cpp
)

target_link_libraries(InsuranceSim PRIVATE Qt6::Widgets)
