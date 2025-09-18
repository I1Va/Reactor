cmake_minimum_required(VERSION 3.10)

project(Reactor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g")


find_package(Qt6 REQUIRED COMPONENTS Core Widgets PrintSupport)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(Reactor
    main.cpp
    inc/reactor.h src/reactor.cpp
    #inc/qcustomplot.h src/qcustomplot.cpp
    inc/record_widget.h src/record_widget.cpp
    inc/reactorcore.h src/reactorcore.cpp
)

target_include_directories(Reactor
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_subdirectory(libs/geometry_module)
target_link_libraries(Reactor PUBLIC geometry_module)

target_link_libraries(Reactor PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::PrintSupport
) 

