cmake_minimum_required(VERSION 3.10)

project(Reactor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(Reactor
    main.cpp
    inc/canvas.hpp src/canvas.cpp
)

target_include_directories(Reactor
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(Reactor PRIVATE
    Qt::Core
    Qt::Widgets
) 

