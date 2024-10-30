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
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core ${CMAKE_CURRENT_BINARY_DIR}/core_build)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Box2D paths (user-provided)
set(BOX2D_INCLUDE_DIR "" CACHE PATH "Path to the Box2D include directory")
set(BOX2D_LIB_DIR_RELEASE "" CACHE FILEPATH "Path to the Box2D release library")
set(BOX2D_LIB_DIR_DEBUG "" CACHE FILEPATH "Path to the Box2D debug library")

# enable utils extension for Box2D
add_definitions(-DBOX2D_UTILS)

# Make Box2D paths mandatory
if(NOT BOX2D_INCLUDE_DIR)
    message(FATAL_ERROR "Box2D include directory not provided. Set BOX2D_INCLUDE_DIR.")
endif()
if(NOT BOX2D_LIB_DIR_RELEASE)
    message(FATAL_ERROR "Box2D release library not provided. Set BOX2D_LIB_DIR_RELEASE.")
endif()
if(NOT BOX2D_LIB_DIR_DEBUG)
    message(FATAL_ERROR "Box2D debug library not provided. Set BOX2D_LIB_DIR_DEBUG.")
endif()

# Include Box2D headers
include_directories(${BOX2D_INCLUDE_DIR})


# utils (header-only) library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core)

# create exe and link
add_executable(${project_name} ${sources})
target_link_libraries(${project_name} SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer agpcore $<IF:$<CONFIG:Debug>,${BOX2D_LIB_DIR_DEBUG},${BOX2D_LIB_DIR_RELEASE}>)