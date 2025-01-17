cmake_minimum_required(VERSION 3.16)

project(chip8 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

find_package(SDL3 REQUIRED COMPONENTS SDL3)

add_executable(chip8 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/keyboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu.cpp)

target_include_directories(chip8 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(chip8 PRIVATE SDL3::SDL3)
