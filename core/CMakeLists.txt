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

# add SDL_TTF support
option(WITH_TTF "Enable SDL_ttf support" OFF)
if (WITH_TTF)
    find_package(SDL2_ttf REQUIRED)

    if (SDL2_ttf_FOUND)
		target_link_libraries(agpcore SDL2_ttf::SDL2_ttf)
		add_definitions(-DWITH_TTF)
    else()
        message(FATAL_ERROR "SDL_ttf support is enabled, but SDL2_ttf library was not found.")
    endif()
endif()

# add shaders support
option(WITH_SHADERS "Enable shaders support" OFF)
if (WITH_SHADERS)
    find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)
	add_definitions(-DWITH_SHADERS)
	
    if (OpenGL_FOUND)
		include_directories(${OPENGL_INCLUDE_DIR})
		target_link_libraries(agpcore ${OPENGL_LIBRARIES})
    else()
        message(FATAL_ERROR "Shaders support is enabled, but OpenGL library was not found.")
    endif()
	
	if (GLEW_FOUND)
		include_directories(${GLEW_INCLUDE_DIRS})
		target_link_libraries(agpcore GLEW::glew)
    else()
        message(FATAL_ERROR "Shaders support is enabled, but GLEW library was not found.")
    endif()
endif()

