# find source files
file(GLOB srcs *.h *.c *.hpp *.cpp)

# libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils)

# create static library from source files
add_library(agpcore ${srcs})
target_link_libraries(agpcore SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)