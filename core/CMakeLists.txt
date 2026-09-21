if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# find source files
file(GLOB srcs *.h *.c *.hpp *.cpp)

# libraries
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils)

# create static library from source files
add_library(agpcore ${srcs} )
target_link_libraries(agpcore SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer)

# add SDL_TTF support
option(WITH_TTF "Enable SDL_ttf support" OFF)
if (WITH_TTF)
    find_package(SDL3_ttf REQUIRED)

    if(APPLE AND TARGET SDL3_ttf::SDL3_ttf-shared)
        get_target_property(SDL3_TTF_LOCATION SDL3_ttf::SDL3_ttf-shared IMPORTED_LOCATION)
        if(NOT SDL3_TTF_LOCATION OR NOT EXISTS "${SDL3_TTF_LOCATION}")
            get_filename_component(SDL3_TTF_FRAMEWORK "${SDL3_ttf_DIR}/../../../.." ABSOLUTE)
            set_target_properties(SDL3_ttf::SDL3_ttf-shared PROPERTIES IMPORTED_LOCATION "${SDL3_TTF_FRAMEWORK}/SDL3_ttf")
        endif()
    endif()

    if (SDL3_ttf_FOUND)
		target_link_libraries(agpcore SDL3_ttf::SDL3_ttf)
		add_definitions(-DWITH_TTF)
    else()
        message(FATAL_ERROR "SDL_ttf support is enabled, but SDL3_ttf library was not found.")
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
