cmake_minimum_required(VERSION 2.6)

project(soil)

SET(SOIL_SRCS
    etc1_utils.c
    image_DXT.c
    image_helper.c
    SOIL2.c
)

if(NOT EMSCRIPTEN AND NOT ANDROID)
	find_package(GLEW REQUIRED)

	find_package(OpenGL REQUIRED)
	include_directories(${OpenGL_INCLUDE_DIRS})
	link_directories(${OpenGL_LIBRARY_DIRS})
	add_definitions(${OpenGL_DEFINITIONS})
	if(NOT OPENGL_FOUND)
		message(ERROR " OPENGL not found!")
	endif(NOT OPENGL_FOUND)
endif()

ADD_LIBRARY(soil STATIC ${SOIL_SRCS})
SET_TARGET_PROPERTIES(soil PROPERTIES OUTPUT_NAME "soil")
SET(SOIL_LIB_TARGET soil)
SET(SOIL_INSTALL_TARGETS ${SOIL_INSTALL_TARGETS} ";soil")
target_link_libraries(soil ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
