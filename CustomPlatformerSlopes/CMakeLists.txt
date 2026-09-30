cmake_minimum_required(VERSION 3.7)

# project name = parent folder name
get_filename_component(project_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${project_name})

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# C++ 14 required
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# get all source files within this folder
file(GLOB sources *.h *.hpp *.cpp *.c)

# libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core ${CMAKE_CURRENT_BINARY_DIR}/core_build)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)

# utils (header-only) library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core)

# create exe and link
add_executable(${project_name} ${sources})
target_link_libraries(${project_name} SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer agpcore)

add_custom_command(TARGET ${project_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/sprites $<TARGET_FILE_DIR:${project_name}>/sprites
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/sounds $<TARGET_FILE_DIR:${project_name}>/assets/sounds
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/musics $<TARGET_FILE_DIR:${project_name}>/assets/musics
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:${project_name}>/assets/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/icons $<TARGET_FILE_DIR:${project_name}>/icons)

# level data can change without recompiling the game
add_custom_target(copy_levels ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/levels $<TARGET_FILE_DIR:${project_name}>/levels
    DEPENDS ${project_name})

# Optional regression checks for slope physics and level/editor integration.
option(SLOPES_BUILD_TESTS "Build slope physics regression tests" OFF)
if(SLOPES_BUILD_TESTS)
    enable_testing()
    set(test_sources ${sources})
    list(REMOVE_ITEM test_sources ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    add_executable(slopes_tests tests/slopes.cpp ${test_sources})
    target_include_directories(slopes_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(slopes_tests SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer agpcore)
    add_dependencies(slopes_tests copy_levels)
    add_test(NAME slopes_physics COMMAND slopes_tests)
    add_test(NAME slopes_level COMMAND slopes_tests --level)
    set_tests_properties(slopes_level PROPERTIES ENVIRONMENT
        "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy;SDL_RENDER_DRIVER=software")
endif()
