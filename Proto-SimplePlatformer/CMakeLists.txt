cmake_minimum_required(VERSION 3.7)

# project name = parent folder name
get_filename_component(project_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${project_name})

# C++ 14 required
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# get all source files within this folder
file(GLOB sources *.h *.hpp *.cpp *.c)

# libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

# utils (header-only) library
get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR}/utils)

# create exe and link
add_executable(${project_name} ${sources})
target_link_libraries(${project_name} SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)